#include "Chunk.h"
#include <cmath>
#include <limits>
#include <unordered_set>
#include <ManagerLocator/ManagerLocator.h>
#include <Managers/RenderManager/RenderManager.h>
#include <DeviceManager.h>
//#include <Assets/Base/MeshAsset.h>
#include <Game/Systems/Terrain/Procedural/Engines/ProceduralEngineBase.h>
#include <Defines/WorldTerrain.h>
#include <Helpers/ChunkHelper.h>

HRESULT Chunk::Init() {
    m_boundingBox.min.x = (float)m_id.x * m_chunkSize;
    m_boundingBox.min.z = (float)m_id.z * m_chunkSize;
    m_boundingBox.min.y = std::numeric_limits<float>::max();

    m_boundingBox.max.x = m_boundingBox.min.x + m_chunkSize;
    m_boundingBox.max.z = m_boundingBox.min.z + m_chunkSize;
    m_boundingBox.max.y = std::numeric_limits<float>::min();

    /*m_mesh = nullptr;*/

    m_currentLOD = -1; // Forzar la generación del LOD 0 en la primera actualización

    return S_OK;
}

void Chunk::UpdateLOD(int lodLevel) {
    if (lodLevel == m_currentLOD) {
        return;
    }
    m_currentLOD = lodLevel;

    m_vertices.clear();
    m_indexes.clear();

    GenerateLODMesh(lodLevel);
}

void Chunk::GenerateLODMesh(int lodLevel) {
    int gridSize = static_cast<int>(m_chunkSize / std::pow(2.0f, lodLevel));
    std::vector<std::shared_ptr<IVertex>> processedVertices;

    //SetNeighborLODs(northLOD, southLOD, eastLOD, westLOD);

    // Aquí se llama a la función que ahora actualiza la AABB
    GenerateVertices(gridSize, lodLevel, processedVertices);

    GenerateIndices(gridSize, lodLevel, m_indexes);

    CalculateNormals(processedVertices);

    for (const auto& v : processedVertices) {
        m_vertices.push_back(v);
    }
}

std::optional<XMFLOAT3> Chunk::InterpolateVertex(float globalX, float globalZ, int gridSize, const TerrainChunk::NeighborDirection& dir, const DirectX::XMFLOAT4& debugColor)
{
    std::shared_ptr<Chunk> neighbor = m_neighbors[static_cast<int>(dir)];
	if (!neighbor) return std::nullopt;
	// !!!!! OJO !!!!!
    // if (neighbor->GetCurrentLOD() <= m_currentLOD || IsDirty()) return std::nullopt;
    if (neighbor->GetCurrentLOD() <= m_currentLOD) return std::nullopt;

    std::vector<std::shared_ptr<IVertex>> vertex = neighbor->GetBorderVertices(dir, gridSize);
	// std::vector<std::shared_ptr<IVertex>> vertex = GetBorderVerticesFromTriangles(dir, 0.01f);
	if (vertex.empty()) return std::nullopt;
    if (vertex.size() == 1) {
        return vertex[0]->GetPosition();
    };
	std::shared_ptr<IVertex> closest1 = nullptr;
	std::shared_ptr<IVertex> closest2 = nullptr;
    if (vertex.size() == 2) {
        closest1 = vertex[0];
        closest2 = vertex[1];
	} else {
		// Buscar los dos vértices más cercanos
		float minDist1 = std::numeric_limits<float>::max();
		float minDist2 = std::numeric_limits<float>::max();
        for (const auto& v : vertex) {
            const XMFLOAT3 pos = v->GetPosition();
            float dist = std::sqrt(std::pow(pos.x - globalX, 2) + std::pow(pos.z - globalZ, 2));
            if (dist < minDist1) {
                minDist2 = minDist1;
                closest2 = closest1;
                minDist1 = dist;
                closest1 = v;
            }
            else if (dist < minDist2) {
                minDist2 = dist;
                closest2 = v;
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

    XMFLOAT3 A = closest1->GetPosition();
    XMFLOAT3 B = closest2->GetPosition();
    XMFLOAT3 AB = DirectXUtils::Subtract(B, A);
    XMFLOAT3 AP = DirectXUtils::Subtract(XMFLOAT3{ globalX, closest2->GetPosition().y, globalZ}, A);
    float t = DirectXUtils::Dot(AP, AB) / DirectXUtils::Dot(AB, AB);
    t = std::clamp(t, 0.0f, 1.0f);
    XMFLOAT3 projected = DirectXUtils::Lerp(A, B, t);
	return projected;
}

void Chunk::GenerateVertices(int gridSize, int lodLevel, std::vector<std::shared_ptr<IVertex>>& outVertices) {
    float scaleFactor = static_cast<float>(std::pow(2.0f, lodLevel));
    outVertices.reserve((gridSize + 1) * (gridSize + 1));

    // Inicializa la altura min/max al inicio de la función
    float minHeight = std::numeric_limits<float>::max();
    float maxHeight = -std::numeric_limits<float>::max();

    for (int z = 0; z <= gridSize; ++z) {
        for (int x = 0; x <= gridSize; ++x) {
            float globalX = (float)m_id.x * m_chunkSize + (float)x * scaleFactor;
            float globalZ = (float)m_id.z * m_chunkSize + (float)z * scaleFactor;

            float height = -1.0f;

            /*DirectX::XMFLOAT4 debugColor = DirectXUtils::GenerateRandomColor();
                        
            TerrainChunk::NeighborDirection dir = TerrainChunk::NeighborDirection::UNDEFINED;
            if (z == gridSize) dir = TerrainChunk::NeighborDirection::EAST;
            else if (z == 0) dir = TerrainChunk::NeighborDirection::WEST;
            else if (x == gridSize) dir = TerrainChunk::NeighborDirection::NORTH;
            else if (x == 0) dir = TerrainChunk::NeighborDirection::SOUTH;*/
            /*else if (x == 0 && z == 0) dir = TerrainChunk::NeighborDirection::SOUTH_WEST;
            else if (x == 0 && z == gridSize) dir = TerrainChunk::NeighborDirection::NORTH_WEST;
            else if (x == gridSize && z == 0) dir = TerrainChunk::NeighborDirection::SOUTH_EAST;
			else if (x == gridSize && z == gridSize) dir = TerrainChunk::NeighborDirection::NORTH_EAST;*/

            std::optional<XMFLOAT3> newPos = std::nullopt;
            
            /*if (dir != TerrainChunk::NeighborDirection::UNDEFINED)
				newPos = InterpolateVertex(globalX, globalZ, gridSize, dir, debugColor);
            
            if (newPos.has_value()) {
				SetDirty(true);
                height = newPos->y;
                globalX = newPos->x;
                globalZ = newPos->z;                    
            }
            else {*/
                height = m_proceduralEngine->GetHeight(globalX, globalZ) * m_terrainHeight;
            //}

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
            
            // DEBUG
            //if (newPos.has_value()) {
            //    //if (dir == TerrainChunk::NeighborDirection::NORTH) newVertex.SetDebugColor(debugColor);
            //    /*if (dir == TerrainChunk::NeighborDirection::NORTH) newVertex.SetDebugColor(DirectX::XMFLOAT4{ 1.0f,1.0f,0.0f,1.0f });*/
            //    /*if (dir == TerrainChunk::NeighborDirection::SOUTH) newVertex.SetDebugColor(DirectX::XMFLOAT4{ 0.0f,1.0f,0.0f,1.0f });
            //    if (dir == TerrainChunk::NeighborDirection::EAST) newVertex.SetDebugColor(DirectX::XMFLOAT4{ 0.0f,0.0f,1.0f,1.0f });
            //    if (dir == TerrainChunk::NeighborDirection::WEST) newVertex.SetDebugColor(DirectX::XMFLOAT4{ 1.0f,0.0f,1.0f,1.0f });*/
            //}
            //if (newPos.has_value()) {
            //if (dir == TerrainChunk::NeighborDirection::NORTH) {
            //    newVertex.SetDebugColor(DirectX::XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f });                
            //}
            //if (IsDirty()) {
            //    newVertex.SetDebugColor(DirectX::XMFLOAT4(0, 0, 1, 1)); // Azul si el chunk está sucio
            //}
            //else {
            //    newVertex.SetDebugColor(DirectX::XMFLOAT4{}); // Azul si el chunk está sucio
            //}
            // 
            //if (lodLevel == 6) {
            //    //newVertex.SetDebugColor(DirectX::XMFLOAT4{ 1.0f,0.0f,1.0f,1.0f });
            //}
            // FIN DE DEBUG
            /*if (x == 0 && z == 0) newVertex.SetDebugColor(DirectX::XMFLOAT4{ 1.0f,0.0f,1.0f,1.0f });
            else if (x == 0 && z == gridSize) newVertex.SetDebugColor(DirectX::XMFLOAT4{ 1.0f,0.0f,1.0f,1.0f });
            else if (x == gridSize && z == 0) newVertex.SetDebugColor(DirectX::XMFLOAT4{ 1.0f,0.0f,1.0f,1.0f });
            else if (x == gridSize && z == gridSize) newVertex.SetDebugColor(DirectX::XMFLOAT4{ 1.0f,0.0f,1.0f,1.0f });*/

            outVertices.push_back(std::make_shared<VertexDefinition::TextureMapVertex>(newVertex));
        }
    }

    // Al final, actualiza la BoundingBox
    m_boundingBox.min.y = minHeight;
    m_boundingBox.max.y = maxHeight;
}

void Chunk::GenerateIndices(int gridSize, int lodLevel, std::vector<UINT>& outIndices) {

    outIndices.clear();
    outIndices.reserve(gridSize * gridSize * 6);
    int verticesPerSide = gridSize + 1;
    for (int z = 0; z < gridSize; ++z) {
        for (int x = 0; x < gridSize; ++x) {
            UINT topLeft = (z * verticesPerSide) + x;
            UINT topRight = topLeft + 1;
            UINT bottomLeft = ((z + 1) * verticesPerSide) + x;
            UINT bottomRight = bottomLeft + 1;

            outIndices.push_back(topLeft);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(topRight);

            outIndices.push_back(topRight);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(bottomRight);
        }
    }
}

void Chunk::CalculateNormals(std::vector<std::shared_ptr<IVertex>>& processedVertices) {
    std::vector<DirectX::XMVECTOR> accumulatedNormals(processedVertices.size(), DirectX::XMVectorZero());

    // Paso 1: acumular normales por triángulo
    for (size_t i = 0; i < m_indexes.size(); i += 3) {
        UINT i0 = m_indexes[i + 0];
        UINT i1 = m_indexes[i + 1];
        UINT i2 = m_indexes[i + 2];

        const XMFLOAT3 pos0 = processedVertices[i0]->GetPosition();
        const XMFLOAT3 pos1 = processedVertices[i1]->GetPosition();
        const XMFLOAT3 pos2 = processedVertices[i2]->GetPosition();

        XMVECTOR p0 = XMLoadFloat3(&pos0);
        XMVECTOR p1 = XMLoadFloat3(&pos1);
        XMVECTOR p2 = XMLoadFloat3(&pos2);

        XMVECTOR edge1 = XMVectorSubtract(p1, p0);
        XMVECTOR edge2 = XMVectorSubtract(p2, p0);
        XMVECTOR faceNormal = XMVector3Normalize(XMVector3Cross(edge1, edge2));

        accumulatedNormals[i0] = XMVectorAdd(accumulatedNormals[i0], faceNormal);
        accumulatedNormals[i1] = XMVectorAdd(accumulatedNormals[i1], faceNormal);
        accumulatedNormals[i2] = XMVectorAdd(accumulatedNormals[i2], faceNormal);
    }

    // Paso 2: suavizar normales con vecinos
    for (size_t i = 0; i < processedVertices.size(); ++i) {
        XMVECTOR normal = accumulatedNormals[i];

        const XMFLOAT3 pos = processedVertices[i]->GetPosition();

        /*
        // Suavizado con vecinos desactivado temporalmente
        for (auto& neighbor : m_neighbors) {
            if (!neighbor) continue;

            auto match = neighbor->FindVertexByPosition(pos, 0.001f);
            if (match) {
                const XMFLOAT3 neighborNormal = match->GetNormal();
                XMVECTOR n = XMLoadFloat3(&neighborNormal);
                normal = XMVectorAdd(normal, n);
            }
        }
        */

        normal = XMVector3Normalize(normal);
        processedVertices[i]->SetNormal(XMFLOAT3(
            XMVectorGetX(normal),
            XMVectorGetY(normal),
            XMVectorGetZ(normal)
        ));
    }
}


std::vector<DirectX::XMFLOAT3> Chunk::GetVerticesVectors() const {
    std::vector<DirectX::XMFLOAT3> vertexPositions;
    vertexPositions.reserve(m_vertices.size());
    for (const auto& vertexVariant : m_vertices) {
        vertexPositions.push_back(vertexVariant->GetPosition());
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

std::shared_ptr<IVertex> Chunk::FindVertexByPosition(const DirectX::XMFLOAT3& position, float tolerance) const
{
    std::shared_ptr<IVertex> closest = nullptr;
    float minDistSq = tolerance * tolerance;

    for (const auto& vertex : m_vertices)
    {
        const DirectX::XMFLOAT3& vPos = vertex->GetPosition(); // Asumiendo que tienes este método
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

std::vector<std::shared_ptr<IVertex>> Chunk::GetBorderVertices(TerrainChunk::NeighborDirection dir, int gridSize) const {
    std::vector<std::shared_ptr<IVertex>> result;

    for (const auto& vertex : m_vertices) {
        const DirectX::XMFLOAT3 pos = vertex->GetPosition();

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
            const XMFLOAT3& pos = m_vertices[idx]->GetPosition();

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

std::vector<std::shared_ptr<IVertex>> Chunk::GetBorderVerticesFromTriangles(int direction, float epsilon) const {
    std::vector<std::shared_ptr<IVertex>> result;
    std::unordered_set<UINT> added;

    auto triangles = GetTrianglesTouchingBorder(direction, epsilon);

    for (const auto& [i0, i1, i2] : triangles) {
        for (UINT idx : { i0, i1, i2 }) {
            const XMFLOAT3& pos = m_vertices[idx]->GetPosition();

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