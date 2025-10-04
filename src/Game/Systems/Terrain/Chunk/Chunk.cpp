#include "Chunk.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <d3d11.h>
#include <Defines/VertexDefinition.h>
#include <DirectXMath.h>
#include <DirectXUtils.h>
#include <Game/Systems/Terrain/Procedural/Engines/ProceduralEngineBase.h>
#include <IDefine/IVertex.h>
#include <limits>
#include <memory>
#include <optional>
#include <unordered_set>
#include <Utils.h>
#include <vector>
#include <wrl/client.h>

// -----------------------------------------------------------------------------
// INIT
// -----------------------------------------------------------------------------
HRESULT Chunk::Init() {
    m_boundingBox.min.x = (float)m_id.x * m_chunkSize;
    m_boundingBox.min.z = (float)m_id.z * m_chunkSize;
    m_boundingBox.min.y = std::numeric_limits<float>::max();

    m_boundingBox.max.x = m_boundingBox.min.x + m_chunkSize;
    m_boundingBox.max.z = m_boundingBox.min.z + m_chunkSize;
    m_boundingBox.max.y = std::numeric_limits<float>::min();

    /*m_mesh = nullptr;*/

    m_currentLOD = -1; // Forzar la generación del LOD 0 en la primera actualización

    m_dirty = true;

    return S_OK;
}

// -----------------------------------------------------------------------------
// SetRegion
// -----------------------------------------------------------------------------
void Chunk::SetRegion(const TerrainChunk::ChunkBufferRegion& region) {
    m_bufferRegion = region;
}

// -----------------------------------------------------------------------------
// Helper para obtener el gridSize actual basado en el LOD.
// Asumiendo que m_chunkSize y m_currentLOD son miembros accesibles.
// -----------------------------------------------------------------------------
int Chunk::GetGridSize() const {
    if (m_currentLOD < 0) return 0; // Manejo seguro si no se ha inicializado
    return static_cast<int>(m_chunkSize / std::pow(2.0f, m_currentLOD));
}

// -----------------------------------------------------------------------------
// SetBuffers
// -----------------------------------------------------------------------------
void Chunk::SetBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, UINT vertexTypeSize) {
    UINT vertexOffsetBytes = m_bufferRegion.vertexOffset * vertexTypeSize;
    UINT indexOffsetBytes = m_bufferRegion.indexOffset * sizeof(uint16_t);

    D3D11_MAPPED_SUBRESOURCE mappedVB;
    context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedVB);
    memcpy((BYTE*)mappedVB.pData + vertexOffsetBytes, m_vertices.data(), m_bufferRegion.vertexCount * static_cast<size_t>(vertexTypeSize));
    context->Unmap(vertexBuffer, 0);

    D3D11_MAPPED_SUBRESOURCE mappedIB;
    context->Map(indexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedIB);
    memcpy((BYTE*)mappedIB.pData + indexOffsetBytes, m_indexes.data(), m_bufferRegion.indexCount * sizeof(uint16_t));
    context->Unmap(indexBuffer, 0);
}

// -----------------------------------------------------------------------------
// UpdateLOD
// -----------------------------------------------------------------------------
void Chunk::UpdateLOD(int lodLevel) {
    if (lodLevel == m_currentLOD) {
        return;
    }
    m_currentLOD = lodLevel;

    m_vertices.clear();
    m_indexes.clear();

    GenerateLODMesh(lodLevel);
}

// -----------------------------------------------------------------------------
// GenerateLODMesh
// -----------------------------------------------------------------------------
void Chunk::GenerateLODMesh(int lodLevel) {
    int gridSize = static_cast<int>(m_chunkSize / std::pow(2.0f, lodLevel));

    // 1. Generar vértices, alturas, UVs y AABB. (Llena m_vertices)
    GenerateVertices(gridSize, lodLevel);

    // 2. Generar índices. (Llena m_indexes)
    GenerateIndices(gridSize, lodLevel);

    // 3. Calcular y aplicar las normales. (Modifica m_vertices)
    CalculateNormals();
}

// -----------------------------------------------------------------------------
// InterpolateVertex
// -----------------------------------------------------------------------------
std::optional<XMFLOAT3> Chunk::InterpolateVertex(float globalX, float globalZ, int gridSize, const TerrainChunk::NeighborDirection& dir, const DirectX::XMFLOAT4& debugColor)
{
    Chunk* neighbor = m_neighbors[static_cast<int>(dir)];
    if (!neighbor) return std::nullopt;
    // !!!!! OJO !!!!!
    // if (neighbor->GetCurrentLOD() <= m_currentLOD || IsDirty()) return std::nullopt;
    if (neighbor->GetCurrentLOD() <= m_currentLOD) return std::nullopt;

    std::vector<IVertex> vertex = neighbor->GetBorderVertices(dir, gridSize);
    // std::vector<std::shared_ptr<IVertex>> vertex = GetBorderVerticesFromTriangles(dir, 0.01f);
    if (vertex.empty()) return std::nullopt;
    if (vertex.size() == 1) {
        return vertex[0].GetPosition();
    };
    IVertex* closest1 = nullptr;
    IVertex* closest2 = nullptr;
    if (vertex.size() == 2) {
        closest1 = &vertex[0];
        closest2 = &vertex[1];
    }
    else {
        // Buscar los dos vértices más cercanos
        float minDist1 = std::numeric_limits<float>::max();
        float minDist2 = std::numeric_limits<float>::max();
        for (auto& v : vertex) {
            const XMFLOAT3 pos = v.GetPosition();
            float dist = std::sqrt(std::pow(pos.x - globalX, 2) + std::pow(pos.z - globalZ, 2));
            if (dist < minDist1) {
                minDist2 = minDist1;
                closest2 = closest1;
                minDist1 = dist;
                closest1 = &v;
            }
            else if (dist < minDist2) {
                minDist2 = dist;
                closest2 = &v;
            }
        }
    }

    /*if (dir == TerrainChunk::NeighborDirection::NORTH) {
        closest1->SetDebugColor(DirectX::XMFLOAT4{ 1.0f,1.0f,0.0f,0.6f });
        closest2->SetDebugColor(DirectX::XMFLOAT4{ 1.0f,1.0f,0.0f,0.6f });
    }
    if (dir == TerrainChunk::NeighborDirection::SOUTH) {
        closest1->SetDebugColor(DirectX::XMFLOAT4{ 0.0f,1.0f,0.0f,0.6f });
        closest2->SetDebugColor(DirectX::XMFLOAT4{ 0.0f,1.0f,0.0f,0.6f });
    }
    if (dir == TerrainChunk::NeighborDirection::EAST) {
        closest1->SetDebugColor(DirectX::XMFLOAT4{ 0.0f,0.0f,1.0f,0.6f });
        closest2->SetDebugColor(DirectX::XMFLOAT4{ 0.0f,0.0f,1.0f,0.6f });
    }

    if (dir == TerrainChunk::NeighborDirection::WEST) {
        closest1->SetDebugColor(DirectX::XMFLOAT4{ 1.0f,0.0f,1.0f,0.6f });
        closest2->SetDebugColor(DirectX::XMFLOAT4{ 1.0f,0.0f,1.0f,0.6f });
    }*/

    /*if (dir == TerrainChunk::NeighborDirection::NORTH) {
        closest1->SetDebugColor(DirectX::XMFLOAT4(0, 1, 0, 1));
        closest2->SetDebugColor(DirectX::XMFLOAT4(0, 1, 0, 1));
    }
    if (dir == TerrainChunk::NeighborDirection::SOUTH) {
        closest1->SetDebugColor(DirectX::XMFLOAT4(1, 0, 0, 1));
        closest2->SetDebugColor(DirectX::XMFLOAT4(1, 0, 0, 1));
    }*/

    XMFLOAT3 A = closest1 != nullptr ? closest1->GetPosition() : XMFLOAT3{};
    XMFLOAT3 B = closest2 != nullptr ? closest2->GetPosition() : XMFLOAT3{};
    XMFLOAT3 AB = DirectXUtils::Subtract(B, A);
    XMFLOAT3 AP = DirectXUtils::Subtract(XMFLOAT3{ globalX, closest2->GetPosition().y, globalZ }, A);
    float t = DirectXUtils::Dot(AP, AB) / DirectXUtils::Dot(AB, AB);
    t = std::clamp(t, 0.0f, 1.0f);
    XMFLOAT3 projected = DirectXUtils::Lerp(A, B, t);
    return projected;
}

// -----------------------------------------------------------------------------
// GenerateVertices: Genera posiciones, UVs y calcula el AABB (Llena m_vertices).
// -----------------------------------------------------------------------------
void Chunk::GenerateVertices(int gridSize, int lodLevel) {
    float scaleFactor = static_cast<float>(std::pow(2.0f, lodLevel));
    int totalVertices = (gridSize + 1) * (gridSize + 1);

    m_vertices.clear();
    m_vertices.reserve(totalVertices);

    float minHeight = std::numeric_limits<float>::max();
    float maxHeight = -std::numeric_limits<float>::max();

    for (int z = 0; z <= gridSize; ++z) {
        for (int x = 0; x <= gridSize; ++x) {


            // ANTES (Potencialmente menos preciso):
            /*float globalX = (float)m_id.x * m_chunkSize + (float)x * scaleFactor;
            float globalZ = (float)m_id.z * m_chunkSize + (float)z * scaleFactor;*/

            // DESPUÉS (Mejorar la precisión de los índices X y Z al sumar):
            // Asegura que la posición global sea consistente usando la suma de indices,
            // no la multiplicación de floats grandes, y luego convirtiendo a float.
            double d_globalX = (double)m_id.x * m_chunkSize + (double)x * scaleFactor;
            double d_globalZ = (double)m_id.z * m_chunkSize + (double)z * scaleFactor;

            float globalX = (float)d_globalX;
            float globalZ = (float)d_globalZ;

            float height = m_proceduralEngine->GetHeight(globalX, globalZ) * m_terrainHeight;

            if (height < minHeight) minHeight = height;
            if (height > maxHeight) maxHeight = height;

            VertexDefinition::TextureMapVertex newVertex{};
            newVertex.position[0] = globalX;
            newVertex.position[1] = height;
            newVertex.position[2] = globalZ;

            newVertex.normal[0] = 0.0f;
            newVertex.normal[1] = 0.0f;
            newVertex.normal[2] = 0.0f;

            newVertex.texCoord[0] = static_cast<float>(x) / gridSize;
            newVertex.texCoord[1] = 1.0f - static_cast<float>(z) / gridSize;
            newVertex.tangent[0] = 1.0f;
            newVertex.tangent[1] = 0.0f;
            newVertex.tangent[2] = 0.0f;

            m_vertices.push_back(newVertex);
        }
    }

    m_boundingBox.min.y = minHeight;
    m_boundingBox.max.y = maxHeight;
}

// -----------------------------------------------------------------------------
// GenerateIndices: Genera el patrón de triángulos (Llena m_indexes).
// -----------------------------------------------------------------------------
void Chunk::GenerateIndices(int gridSize, int lodLevel) { // La firma ya no acepta 'outIndices'
    // La variable lodLevel ya no se usa, pero se mantiene en la firma por consistencia.

    m_indexes.clear();
    m_indexes.reserve(gridSize * gridSize * 6);
    int verticesPerSide = gridSize + 1;

    for (int z = 0; z < gridSize; ++z) {
        for (int x = 0; x < gridSize; ++x) {
            UINT topLeft = (z * verticesPerSide) + x;
            UINT topRight = topLeft + 1;
            UINT bottomLeft = ((z + 1) * verticesPerSide) + x;
            UINT bottomRight = bottomLeft + 1;

            // Triángulo 1
            m_indexes.push_back(topLeft);
            m_indexes.push_back(bottomLeft);
            m_indexes.push_back(topRight);

            // Triángulo 2
            m_indexes.push_back(topRight);
            m_indexes.push_back(bottomLeft);
            m_indexes.push_back(bottomRight);
        }
    }
}

// -----------------------------------------------------------------------------
// CalculateNormals
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CalculateNormals: Calcula las normales por vértice (Modifica m_vertices).
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CalculateNormals: Calcula las normales por vértice (Modifica m_vertices).
// -----------------------------------------------------------------------------
void Chunk::CalculateNormals() {

    // Inicializar un vector temporal para acumular las normales de cara.
    std::vector<DirectX::XMVECTOR> accumulatedNormals(m_vertices.size(), DirectX::XMVectorZero());

    // Paso 1: acumular normales por triángulo.
    for (size_t i = 0; i < m_indexes.size(); i += 3) {
        UINT i0 = m_indexes[i + 0];
        UINT i1 = m_indexes[i + 1];
        UINT i2 = m_indexes[i + 2];

        // CORRECCIÓN CLAVE: Cargar XMVECTOR directamente desde el array float[3] (position).
        // Usamos reinterpret_cast para tratar el array float[3] como un puntero a XMFLOAT3,
        // o si es una estructura simple, cargamos el vector SIMD directamente.

        // Opción 1 (Más limpia, asume el layout es compatible con XMFLOAT3):
        XMVECTOR p0 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(m_vertices[i0].position));
        XMVECTOR p1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(m_vertices[i1].position));
        XMVECTOR p2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(m_vertices[i2].position));

        /*
        // Opción 2 (Si no quieres el reinterpret_cast, creas un XMFLOAT3 temporal):
        const XMFLOAT3 pos0 = { m_vertices[i0].position[0], m_vertices[i0].position[1], m_vertices[i0].position[2] };
        XMVECTOR p0 = XMLoadFloat3(&pos0);
        */

        XMVECTOR edge1 = XMVectorSubtract(p1, p0);
        XMVECTOR edge2 = XMVectorSubtract(p2, p0);

        XMVECTOR faceNormal = XMVector3Normalize(XMVector3Cross(edge1, edge2));

        accumulatedNormals[i0] = XMVectorAdd(accumulatedNormals[i0], faceNormal);
        accumulatedNormals[i1] = XMVectorAdd(accumulatedNormals[i1], faceNormal);
        accumulatedNormals[i2] = XMVectorAdd(accumulatedNormals[i2], faceNormal);
    }

    // Paso 2: suavizar y normalizar las normales acumuladas.
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        XMVECTOR normal = accumulatedNormals[i];

        /* Aquí se reintroduce la lógica de suavizado con vecinos si es necesario. */

        normal = XMVector3Normalize(normal);

        // Escribir el resultado de vuelta en el array de la normal (normal[3]).
        // Necesitamos un XMFLOAT3 temporal para usar XMStoreFloat3, 
        // luego copiamos los datos al array [3].
        XMFLOAT3 tempNormal;
        XMStoreFloat3(&tempNormal, normal);

        m_vertices[i].normal[0] = tempNormal.x;
        m_vertices[i].normal[1] = tempNormal.y;
        m_vertices[i].normal[2] = tempNormal.z;
    }
}

// -----------------------------------------------------------------------------
// Otros métodos
// -----------------------------------------------------------------------------
std::vector<DirectX::XMFLOAT3> Chunk::GetVerticesVectors() const {
    std::vector<DirectX::XMFLOAT3> vertexPositions;
    vertexPositions.reserve(m_vertices.size());
    for (const auto& vertexVariant : m_vertices) {
        vertexPositions.push_back(vertexVariant.GetPosition());
    }
    return vertexPositions;
}

float Chunk::GetDistanceToCamera(std::shared_ptr<ICamera> camera) const {
    float chunkCenterX = (float)m_id.x * m_chunkSize + m_chunkSize / 2.0f;
    float chunkCenterZ = (float)m_id.z * m_chunkSize + m_chunkSize / 2.0f;
    float cameraX = camera->GetPosition().x;
    float cameraZ = camera->GetPosition().z;
    return std::sqrt(std::pow(chunkCenterX - cameraX, 2) + std::pow(chunkCenterZ - cameraZ, 2));
}

IVertex Chunk::FindVertexByPosition(const DirectX::XMFLOAT3& position, float tolerance) const
{
    IVertex closest = {};
    float minDistSq = tolerance * tolerance;

    for (const auto& vertex : m_vertices)
    {
        const DirectX::XMFLOAT3& vPos = vertex.GetPosition(); // Asumiendo que tienes este método
        float dx = vPos.x - position.x;
        float dy = vPos.y - position.y;
        float dz = vPos.z - position.z;
        float distSq = dx * dx + dy * dy + dz * dz;

        if (distSq <= minDistSq)
        {
            minDistSq = distSq;
            closest = vertex;
        }
    }

    return closest;
}

std::vector<IVertex> Chunk::GetBorderVertices(TerrainChunk::NeighborDirection dir, int gridSize) const {
    std::vector<IVertex> result;

    for (const auto& vertex : m_vertices) {
        const DirectX::XMFLOAT3 pos = vertex.GetPosition();

        switch (dir) {
        case TerrainChunk::NeighborDirection::WEST: if (std::abs(pos.z - m_boundingBox.min.z) < EPSILON) result.push_back(vertex); break;
        case TerrainChunk::NeighborDirection::EAST: if (std::abs(pos.z - m_boundingBox.max.z) < EPSILON) result.push_back(vertex); break;
        case TerrainChunk::NeighborDirection::SOUTH: if (std::abs(pos.x - m_boundingBox.max.x) < EPSILON) result.push_back(vertex); break;
        case TerrainChunk::NeighborDirection::NORTH: if (std::abs(pos.x - m_boundingBox.min.x) < EPSILON) result.push_back(vertex); break;
        }
    }

    if (!result.empty()) {
        bool a = false;
    }

    return result;
}

void Chunk::Shutdown() {
    /*if (m_mesh) {
        m_mesh->Shutdown();
        m_mesh.reset();
    }*/
    m_vertices.clear();
    m_indexes.clear();
    //delete(m_proceduralEngine);
}

std::vector<std::tuple<UINT, UINT, UINT>> Chunk::GetTrianglesTouchingBorder(int direction, float epsilon) const {
    std::vector<std::tuple<UINT, UINT, UINT>> result;

    for (size_t i = 0; i < m_indexes.size(); i += 3) {
        UINT i0 = m_indexes[i];
        UINT i1 = m_indexes[i + 1];
        UINT i2 = m_indexes[i + 2];

        int countOnBorder = 0;

        for (UINT idx : { i0, i1, i2 }) {
            const XMFLOAT3& pos = m_vertices[idx].GetPosition();

            switch (direction) {
            case 0: // NORTH
                if (std::abs(pos.x - m_boundingBox.min.x) < epsilon) countOnBorder++;
                break;
            case 1: // SOUTH
                if (std::abs(pos.x - m_boundingBox.max.x) < epsilon) countOnBorder++;
                break;
            case 2: // EAST
                if (std::abs(pos.z - m_boundingBox.max.z) < epsilon) countOnBorder++;
                break;
            case 3: // WEST
                if (std::abs(pos.z - m_boundingBox.min.z) < epsilon) countOnBorder++;
                break;
            }
        }

        if (countOnBorder >= 2) {
            result.emplace_back(i0, i1, i2);
        }
    }

    return result;
}

std::vector<IVertex> Chunk::GetBorderVerticesFromTriangles(int direction, float epsilon) const {
    std::vector<IVertex> result;
    std::unordered_set<UINT> added;

    auto triangles = GetTrianglesTouchingBorder(direction, epsilon);

    for (const auto& [i0, i1, i2] : triangles) {
        for (UINT idx : { i0, i1, i2 }) {
            const XMFLOAT3& pos = m_vertices[idx].GetPosition();

            bool isOnBorder = false;
            switch (direction) {
            case 0: isOnBorder = std::abs(pos.x - m_boundingBox.min.x) < epsilon; break;
            case 1: isOnBorder = std::abs(pos.x - m_boundingBox.max.x) < epsilon; break;
            case 2: isOnBorder = std::abs(pos.z - m_boundingBox.max.z) < epsilon; break;
            case 3: isOnBorder = std::abs(pos.z - m_boundingBox.min.z) < epsilon; break;
            }

            if (isOnBorder && added.insert(idx).second) {
                result.push_back(m_vertices[idx]);
            }
        }
    }

    return result;
}

// -----------------------------------------------------------------------------
// GetLocalIndex: Calcula el índice en m_vertices[N] dada la posición (x, z) 
// en la cuadrícula local (donde 0 <= x, z <= gridSize).
// -----------------------------------------------------------------------------
size_t Chunk::GetLocalIndex(int x, int z) const {
    int gridSize = GetGridSize();

    // Si la cuadrícula no está generada o no es válida, devolvemos 0 (o lanzamos excepción)
    if (gridSize <= 0) {
        // ¡Ojo! El código anterior devolvía 0. Es mejor devolver un valor seguro o fallar.
        // Pero para ser consistente con la topología, devolvemos 0 si no hay gridSize
        // o asumimos que esto solo se llama después de UpdateLOD.
        return 0;
    }

    int verticesPerSide = gridSize + 1;

    // Se asume que 0 <= x <= gridSize y 0 <= z <= gridSize

    // La fórmula es: (fila * columnas) + columna
    return (size_t)(z * verticesPerSide) + x;
}

// Sólo para uso de ChunkService (friend).
// -----------------------------------------------------------------------------
void Chunk::ApplyStitchedNormalAndTangent(size_t localIndex, DirectX::XMVECTOR normal, DirectX::XMVECTOR tangent) {
    if (localIndex >= m_vertices.size()) {
        return;
    }

    // NORMAL
    DirectX::XMFLOAT3 tempNormal;
    DirectX::XMStoreFloat3(&tempNormal, normal);

    m_vertices[localIndex].normal[0] = tempNormal.x;
    m_vertices[localIndex].normal[1] = tempNormal.y;
    m_vertices[localIndex].normal[2] = tempNormal.z;

    // TANGENTE
    DirectX::XMFLOAT3 tempTangent;
    DirectX::XMStoreFloat3(&tempTangent, tangent);

    m_vertices[localIndex].tangent[0] = tempTangent.x;
    m_vertices[localIndex].tangent[1] = tempTangent.y;
    m_vertices[localIndex].tangent[2] = tempTangent.z;
}
