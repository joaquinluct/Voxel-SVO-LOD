#include "GeometryChunkEngine.h"
#include <Defines/TerrainChunk.h>
#include <Defines/VertexDefinition.h>
#include <DirectXMath.h>
#include <DirectXMathConvert.inl>
#include <DirectXMathVector.inl>
#include <functional>
#include <Game/System/TerrainConfig.h>
#include <Locators/Registers/REGISTER_TERRAIN_ENGINE_MACRO.h>
#include <unordered_map>
#include <vector>
#include <Windows.h>

REGISTER_TERRAIN_ENGINE_TYPE(GeometryChunkEngine, "GeometryChunkEngine") // Temporalmente deshabilitado - archivo obsoleto

void GeometryChunkEngine::Init(TerrainConfig* terrainConfig)
{
    m_config = terrainConfig;
    // TODO: Implementar correctamente cuando se necesite este engine
    // m_meshAssetName = config.terrain_asset;
}

size_t GeometryChunkEngine::HashVertex(const VertexDefinition::TextureMapVertex& v) const {
    constexpr float epsilon = 0.0001f;
    int x = static_cast<int>(v.position[0] / epsilon);
    int y = static_cast<int>(v.position[1] / epsilon);
    int z = static_cast<int>(v.position[2] / epsilon);
    return std::hash<int>()(x) ^ std::hash<int>()(y << 1) ^ std::hash<int>()(z << 2);
}

void GeometryChunkEngine::GenerateMesh(
    std::vector<VertexDefinition::TextureMapVertex>& globalVertexPool,
    std::unordered_map<size_t, UINT>& vertexLookup,
    std::vector<UINT>& index,
    const TerrainChunk::ChunkID& id,
    int zone)
{
    constexpr int gridSize = 32;
    const float scale = 1.0f;
    const int verticesPerSide = gridSize + 1;

    std::vector<std::vector<UINT>> vertexIndexGrid(verticesPerSide, std::vector<UINT>(verticesPerSide));

    // Paso 1: generar vértices y deduplicar
    for (int z = 0; z <= gridSize; ++z) {
        for (int x = 0; x <= gridSize; ++x) {
            float worldX = id.x * gridSize * scale + x * scale;
            float worldZ = id.z * gridSize * scale + z * scale;
            float height = 1.0f; // m_proceduralEngine->GetHeight(worldX, worldZ);

            VertexDefinition::TextureMapVertex v{};
            v.position[0] = worldX;
            v.position[1] = height;
            v.position[2] = worldZ;
            v.normal[0] = 0.0f; // se calculará después
            v.normal[1] = 0.0f;
            v.normal[2] = 0.0f;
            v.texCoord[0] = static_cast<float>(x) / gridSize;
            v.texCoord[1] = 1.0f - static_cast<float>(z) / gridSize;
            v.tangent[0] = 0.0f; // opcional
            v.tangent[1] = 0.0f;
            v.tangent[2] = 0.0f;

            size_t hash = HashVertex(v);
            auto it = vertexLookup.find(hash);
            UINT indexValue;

            if (it != vertexLookup.end()) {
                indexValue = it->second;
            }
            else {
                indexValue = static_cast<UINT>(globalVertexPool.size());
                globalVertexPool.push_back(v);
                vertexLookup[hash] = indexValue;
            }

            vertexIndexGrid[z][x] = indexValue;
        }
    }

    // Paso 2: generar índices
    for (int z = 0; z < gridSize; ++z) {
        for (int x = 0; x < gridSize; ++x) {
            UINT topLeft = vertexIndexGrid[z][x];
            UINT topRight = vertexIndexGrid[z][x + 1];
            UINT bottomLeft = vertexIndexGrid[z + 1][x];
            UINT bottomRight = vertexIndexGrid[z + 1][x + 1];

            index.push_back(topLeft);
            index.push_back(bottomLeft);
            index.push_back(topRight);

            index.push_back(topRight);
            index.push_back(bottomLeft);
            index.push_back(bottomRight);
        }
    }

    // Paso 3: calcular normales por triángulo
    for (size_t i = 0; i < index.size(); i += 3) {
        UINT i0 = index[i];
        UINT i1 = index[i + 1];
        UINT i2 = index[i + 2];

        auto& v0 = globalVertexPool[i0];
        auto& v1 = globalVertexPool[i1];
        auto& v2 = globalVertexPool[i2];

        XMVECTOR p0 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(v0.position));
        XMVECTOR p1 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(v1.position));
        XMVECTOR p2 = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(v2.position));

        XMVECTOR edge1 = XMVectorSubtract(p1, p0);
        XMVECTOR edge2 = XMVectorSubtract(p2, p0);
        XMVECTOR normal = XMVector3Cross(edge1, edge2);

        XMFLOAT3 n;
        XMStoreFloat3(&n, XMVector3Normalize(normal));

        for (UINT idx : { i0, i1, i2 }) {
            globalVertexPool[idx].normal[0] += n.x;
            globalVertexPool[idx].normal[1] += n.y;
            globalVertexPool[idx].normal[2] += n.z;
        }
    }

    // Paso 4: normalizar normales
    for (auto& v : globalVertexPool) {
        XMVECTOR n = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(v.normal));
        n = XMVector3Normalize(n);
        XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(v.normal), n);
    }
}

std::string GeometryChunkEngine::GetMeshAssetName() const {
    return "GeometryChunkMesh"; // Nombre temporal
}

