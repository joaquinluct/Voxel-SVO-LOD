#include "ChunkService.h"
#include <Assets/Base/TerrainAsset.h>
#include <cmath>
#include <d3d11.h>
#include <Defines/Contants/Flags.h>
#include <Defines/VertexDefinition.h>
#include <Game/Systems/Terrain/Chunk/Chunk.h>
#include <Helpers/FrustumCullingHelper.h>
#include <Helpers/PointerQueryRaw.h>
#include <ManagerLocator/ManagerLocator.h>
#include <REGISTER_SERVICE_MACRO.h>
#include <wrl/client.h>

REGISTER_SERVICE_TYPE(ChunkService, "ChunkService")

HRESULT ChunkService::Init(WorldTerrain::TerrainChunkDefinition definition)
{
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (!m_deviceManager) {
        return E_FAIL;
    }
    m_cameraManager = ManagerLocator::GetCameraManager();
    if (!m_cameraManager) {
        return E_FAIL;
    }
    //m_cameraManager->ExtractCurrentFrustumPlanes(m_frustumPlanes);
    m_def = definition;
    if (m_def.chunSize <= 0) {
        return E_INVALIDARG;
    }
    m_chunkSize = m_def.chunSize;
    m_renderDistanceChunks = m_def.initialRenderDistanceChunks;
    return S_OK;
}

Chunk* ChunkService::GetChunk(const TerrainChunk::ChunkID& id) const {
    auto it = m_chunks.find(id);
    if (it != m_chunks.end()) {
        return it->second;
    }
    return nullptr;
}

Chunk* ChunkService::GetNeighbor(const TerrainChunk::ChunkID& id, TerrainChunk::NeighborDirection direction) {
    TerrainChunk::ChunkID neighborId = id;
    switch (direction) {
    case TerrainChunk::NeighborDirection::NORTH: neighborId.x += 1; break;
    case TerrainChunk::NeighborDirection::SOUTH: neighborId.x -= 1; break;
    case TerrainChunk::NeighborDirection::EAST:  neighborId.z += 1; break;
    case TerrainChunk::NeighborDirection::WEST:  neighborId.z -= 1; break;
    }
    Chunk* neighbor = GetChunk(neighborId);
    if (!neighbor) {
        // Intentar cargar el chunk vecino si no está cargado
        HRESULT hr = LoadChunk(id);
    }
    return GetChunk(neighborId);
}

std::vector<Chunk*> ChunkService::GetNeighbors(const TerrainChunk::ChunkID& id) {
    Chunk* northNeighbor = GetNeighbor(id, TerrainChunk::NeighborDirection::NORTH);
    Chunk* southNeighbor = GetNeighbor(id, TerrainChunk::NeighborDirection::SOUTH);
    Chunk* eastNeighbor = GetNeighbor(id, TerrainChunk::NeighborDirection::EAST);
    Chunk* westNeighbor = GetNeighbor(id, TerrainChunk::NeighborDirection::WEST);

    Chunk* neighbors[4] = { northNeighbor, southNeighbor, eastNeighbor, westNeighbor, };

    return std::vector<Chunk*>(neighbors, neighbors + 4);
}

HRESULT ChunkService::LoadChunk(const TerrainChunk::ChunkID& id)
{
    if (m_chunks.count(id)) {
        return S_OK;
    }

    Chunk* newChunk = new Chunk(id);
    if (!newChunk) {
        return E_FAIL;
    }

    newChunk->SetProceduralEngine(m_proceduralService->GetEngine());
    newChunk->SetChunkSize(m_chunkSize);
    newChunk->Init();
    newChunk->SetFlag(FLAG_CHUNK_WITH_VERTEX, false);
    newChunk->SetFlag(FLAG_CHUNK_WITH_VERTEX_BUFFER, false);
    m_chunks[id] = newChunk;
    return S_OK;
}

HRESULT ChunkService::UnloadChunk(const TerrainChunk::ChunkID& id) {
    auto it = m_chunks.find(id);
    if (it != m_chunks.end()) {
        it->second->Shutdown();
        m_chunks.erase(it);
        return S_OK;
    }
    return E_FAIL;
}

//void ChunkService::StitchChunks() {
//    // Itera sobre todos los chunks
//    for (auto& chunkMap : m_chunks) {
//		auto& id = chunkMap.first;
//		auto& chunk = chunkMap.second;
//        // Itera sobre sus vecinos
//
//        for (auto& neighbor : GetNeighbors(id)) {
//            // Si el chunk y el vecino están adyacentes
//            // Obtén los vértices de la costura
//            std::vector<Vertex> chunkVertices = chunk->GetBorderVertices(neighbor);
//            std::vector<Vertex> neighborVertices = neighbor->GetBorderVertices(chunk);
//
//            // Recalcula las normales de los vértices de la costura
//            for (size_t i = 0; i < chunkVertices.size(); ++i) {
//                Vertex& v1 = chunkVertices[i];
//                Vertex& v2 = neighborVertices[i];
//
//                // Asegúrate de que las normales de los vértices adyacentes sean idénticas
//                v1.normal = v2.normal;
//                }
//        }
//    }
//}


// ------------------------------------------------------------------------------------------------------------
// Actualiza los chunks basándose en la posición del jugador y la distancia de renderizado
// ------------------------------------------------------------------------------------------------------------
void ChunkService::UpdateChunks(DirectX::XMFLOAT3 worldPosition)
{
    /*for (auto const& [id, chunkPtr] : m_chunks)
    {
        if (chunkPtr && chunkPtr->IsDirty()) {
            std::vector<Chunk*> neighbors = GetNeighbors(id);
            chunkPtr->SetNeighbors(neighbors.data());
            chunkPtr->UpdateLOD(chunkPtr->GetCurrentLOD());
            chunkPtr->SetDirty(false);
        }
    }*/

    /*if (IsDirty()) {
        bool a = false;
    }*/


    Microsoft::WRL::ComPtr<ID3D11Device> device = m_deviceManager->GetDevice();
    int playerChunkX = static_cast<int>(std::floor(worldPosition.x / m_chunkSize));
    int playerChunkY = static_cast<int>(std::floor(worldPosition.y / m_chunkSize));
    int playerChunkZ = static_cast<int>(std::floor(worldPosition.z / m_chunkSize));

    for (int x = playerChunkX - m_renderDistanceChunks; x <= playerChunkX + m_renderDistanceChunks; ++x) {
        for (int z = playerChunkZ - m_renderDistanceChunks; z <= playerChunkZ + m_renderDistanceChunks; ++z) {
            TerrainChunk::ChunkID id = { x, 0, z };

            float chunkCenterX = (float)x * m_chunkSize + m_chunkSize / 2.0f;
            float chunkCenterZ = (float)z * m_chunkSize + m_chunkSize / 2.0f;

            float distanceX = worldPosition.x - chunkCenterX;
            float distanceZ = worldPosition.z - chunkCenterZ;
            float distance = std::sqrt(distanceX * distanceX + distanceZ * distanceZ);

            if (distance <= m_renderDistanceChunks * m_chunkSize)
            {
                int lodLevel = 6;
                /*if (distance >= m_chunkSize * 80.0f) {
                    lodLevel = 6;
                }*/

                /*int lodLevel = 2;

                if (distance >= m_chunkSize * 5.0f) {
                    lodLevel = 3;
                }

                if (distance >= m_chunkSize * 10.0f) {
                    lodLevel = 4;
                }

                if (distance >= m_chunkSize * 20.0f) {
                    lodLevel = 5;
                }*/

                if (!m_chunks.count(id))
                {
                    LoadChunk(id);
                }

                Chunk* chunk = GetChunk(id);
                bool create = !chunk->GetFlag(FLAG_CHUNK_WITH_VERTEX) || chunk->GetCurrentLOD() != lodLevel;
                if (create) {
                    chunk->SetFlag(FLAG_CHUNK_WITH_VERTEX, true);
                    chunk->SetFlag(FLAG_CHUNK_WITH_VERTEX_BUFFER, false);
                    //std::vector<Chunk*> neighbors = GetNeighbors(id);
                    //chunk->SetNeighbors(neighbors.data());
                    chunk->UpdateLOD(lodLevel);
                }

            }
        }
    }

    //StitchAllChunks();

    std::vector<TerrainChunk::ChunkID> chunksToUnload;
    for (auto const& [id, chunkPtr] : m_chunks)
    {
        float chunkCenterX = (float)id.x * m_chunkSize + m_chunkSize / 2.0f;
        float chunkCenterZ = (float)id.z * m_chunkSize + m_chunkSize / 2.0f;

        float distanceX = worldPosition.x - chunkCenterX;
        float distanceZ = worldPosition.z - chunkCenterZ;
        float distance = std::sqrt(distanceX * distanceX + distanceZ * distanceZ);

        if (distance > m_renderDistanceChunks * m_chunkSize)
        {
            chunksToUnload.push_back(id);
            std::lock_guard<std::mutex> lock(m_chunkDeletionMutex);
            m_chunksToDelete.push_back(chunkPtr);
        }
    }
    for (const auto& id : chunksToUnload)
    {
        m_chunks.erase(id);
    }
}

const bool ChunkService::IsDirty() const {
    for (const auto& pair : m_chunks) {
        if (!pair.second->GetFlag(FLAG_CHUNK_WITH_VERTEX)) {
            return true;
        }
    }
    return false;
}

void ChunkService::EmptyRecycleBin() {
    DirectX::XMFLOAT3 cameraPosition = m_cameraManager->GetCurrentCameraPosition();

    std::lock_guard<std::mutex> lock(m_chunkDeletionMutex);

    auto it = m_chunksToDelete.begin();
    while (it != m_chunksToDelete.end()) {
        auto chunkPtr = *it;
        if (chunkPtr) {
            DirectX::XMFLOAT3 chunkCenter = chunkPtr->GetPosition();
            chunkCenter.x += m_chunkSize / 2.0f;
            chunkCenter.y += m_chunkSize / 2.0f;
            chunkCenter.z += m_chunkSize / 2.0f;

            float dx = cameraPosition.x - chunkCenter.x;
            float dy = cameraPosition.y - chunkCenter.y;
            float dz = cameraPosition.z - chunkCenter.z;
            float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

            if (distance > (m_renderDistanceChunks * m_chunkSize)) {
                m_terrainAsset->UnregisterChunk(chunkPtr);
                chunkPtr->Shutdown();
                it = m_chunksToDelete.erase(it);
                continue;
            }
        }
        ++it;
    }
}

std::vector<Chunk*> ChunkService::GetAllChunks() {
    std::vector<Chunk*> chunks;
    for (auto const& [id, chunk] : m_chunks) {
        if (chunk) {
            chunks.push_back(chunk);
        }
    }
    return chunks;
}

std::vector<Chunk*> ChunkService::GetVisibleChunks() {
    PointerQueryRaw<Chunk> query(GetAllChunks());
    auto visibleChunks = query
        .Filter([](Chunk* c) { return c->GetFlag(FLAG_CHUNK_VISIBLE); })
        //.Sort([](Chunk* a, Chunk* b) { return a->GetDistanceToCamera() < b->GetDistanceToCamera(); })
        .ToVector();

    return visibleChunks;
}

DirectX::XMFLOAT3 ChunkService::GetTerrainNormal(float posX, float posZ) const {
    int chunkX = static_cast<int>(std::floor(posX / m_chunkSize));
    int chunkZ = static_cast<int>(std::floor(posZ / m_chunkSize));
    TerrainChunk::ChunkID id = { chunkX, 0, chunkZ };
    Chunk* chunk = GetChunk(id);
    if (!chunk) {
        return { 0.0f, 1.0f, 0.0f }; // Normal predeterminada si el chunk no está cargado
    }
    DirectX::XMFLOAT3 localPos{};
    localPos.x = posX - (float)chunkX * m_chunkSize;
    localPos.y = 0.0f; // La coordenada Y no es relevante para la normal
    localPos.z = posZ - (float)chunkZ * m_chunkSize;
    IVertex vertex = chunk->FindVertexByPosition(localPos, 0.1f);
    return vertex.GetNormal();
}

std::vector<Chunk*> ChunkService::GetFrustumChunks(const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes, const DirectX::XMFLOAT3 cameraPosition)
{
    std::vector<Chunk*> visibleChunks;

    for (auto const& [id, chunk] : m_chunks)
    {
        chunk->SetFlag(FLAG_CHUNK_VISIBLE, false);
        if (chunk)
        {
            DirectX::XMFLOAT3 minCorner;
            minCorner.x = (float)id.x * m_chunkSize;
            minCorner.y = (float)id.y * m_chunkSize;
            minCorner.z = (float)id.z * m_chunkSize;

            DirectX::XMFLOAT3 maxCorner;
            maxCorner.x = minCorner.x + m_chunkSize;
            maxCorner.y = minCorner.y + m_chunkSize;
            maxCorner.z = minCorner.z + m_chunkSize;

            if (FrustumCullingHelper::IsAABBInFrustum(minCorner, maxCorner, frustumPlanes))
            {
                chunk->SetFlag(FLAG_CHUNK_VISIBLE, true);
                visibleChunks.push_back(chunk);
            }
        }
    }

    return visibleChunks;
}

// -----------------------------------------------------------------------------
// StitchChunkBorder: Une Normales y Tangentes de dos chunks adyacentes.
// ESTA FUNCIÓN DEBE SER LLAMADA DESPUÉS DE Chunk::CalculateNormals() en AMBOS chunks.
// -----------------------------------------------------------------------------
void ChunkService::StitchChunkBorder(Chunk* chunkA, Chunk* chunkB, TerrainChunk::NeighborDirection dirA) {
    if (!chunkA || !chunkB) return;

    int gridSize = chunkA->GetGridSize();

    // Si los LODs son diferentes, el número de vértices en la costura no coincide.
    // La solución para T-junctions es más compleja y está fuera del alcance 
    // de este stitching simétrico. Por ahora, solo unimos si el LOD es el mismo.
    if (gridSize != chunkB->GetGridSize()) {
        return;
    }

    // Indices locales de los vértices en las costuras de la cuadrícula (0 a gridSize)
    int x_a = 0, z_a = 0; // Coordenadas de la costura en Chunk A
    int x_b = 0, z_b = 0; // Coordenadas de la costura en Chunk B

    for (int i = 0; i <= gridSize; ++i) {

        switch (dirA) {
        case TerrainChunk::NeighborDirection::EAST: // Chunk B está al Este de Chunk A (+Z)
            // Chunk A: Borde Este (X=GridSize, Z=i)
            // Chunk B: Borde Oeste (X=0, Z=i)
            x_a = gridSize; z_a = i;
            x_b = 0; z_b = i;
            break;

        case TerrainChunk::NeighborDirection::WEST: // Chunk B está al Oeste de Chunk A (-Z)
            // Chunk A: Borde Oeste (X=0, Z=i)
            // Chunk B: Borde Este (X=GridSize, Z=i)
            x_a = 0; z_a = i;
            x_b = gridSize; z_b = i;
            break;

        case TerrainChunk::NeighborDirection::NORTH: // Chunk B está al Norte de Chunk A (-X)
            // Chunk A: Borde Norte (X=i, Z=0)
            // Chunk B: Borde Sur (X=i, Z=GridSize)
            x_a = i; z_a = 0;
            x_b = i; z_b = gridSize;
            break;

        case TerrainChunk::NeighborDirection::SOUTH: // Chunk B está al Sur de Chunk A (+X)
            // Chunk A: Borde Sur (X=i, Z=GridSize)
            // Chunk B: Borde Norte (X=i, Z=0)
            x_a = i; z_a = gridSize;
            x_b = i; z_b = 0;
            break;
        }

        // Obtenemos los índices de almacenamiento en el vector m_vertices
        size_t indexA = chunkA->GetLocalIndex(x_a, z_a);
        size_t indexB = chunkB->GetLocalIndex(x_b, z_b);

        // --- 1. Cargar Normales y Tangentes ---
        // Accedemos a m_vertices directamente porque ChunkService es 'friend' de Chunk.
        XMVECTOR normalA = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(chunkA->m_vertices[indexA].normal));
        XMVECTOR normalB = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(chunkB->m_vertices[indexB].normal));
        XMVECTOR tangentA = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(chunkA->m_vertices[indexA].tangent));
        XMVECTOR tangentB = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(chunkB->m_vertices[indexB].tangent));

        // --- 2. Promediado y Normalización (la clave para eliminar los escalones) ---
        XMVECTOR stitchedNormal = XMVector3Normalize(XMVectorAdd(normalA, normalB));
        XMVECTOR stitchedTangent = XMVector3Normalize(XMVectorAdd(tangentA, tangentB));

        // --- 3. Aplicar los resultados a AMBOS chunks ---
        chunkA->ApplyStitchedNormalAndTangent(indexA, stitchedNormal, stitchedTangent);
        chunkB->ApplyStitchedNormalAndTangent(indexB, stitchedNormal, stitchedTangent);
    }
}

// -----------------------------------------------------------------------------
// Función principal para la costura
// -----------------------------------------------------------------------------
void ChunkService::StitchAllChunks() {
    // Itera sobre todos los chunks cargados.
    for (auto const& [id, chunkA] : m_chunks) {
        if (chunkA->GetCurrentLOD() == -1) continue; // Saltar chunks sin inicializar

        // Obtenemos los 4 vecinos
        for (int i = 0; i < 4; ++i) {
            TerrainChunk::NeighborDirection dirA = (TerrainChunk::NeighborDirection)i;

            Chunk* chunkB = GetNeighbor(id, dirA);

            // Solo procesamos una vez por par de chunks (ej. A->B pero no B->A)
            // Esto evita el doble procesamiento y es más limpio.
            // Para eso, solo procesamos si el ID del vecino es "menor" o "más lejano"
            // Por simplicidad en IDs de cuadrícula: solo procesar Norte y Este.
            if (dirA == TerrainChunk::NeighborDirection::SOUTH ||
                dirA == TerrainChunk::NeighborDirection::WEST) {
                continue;
            }

            if (chunkB && chunkB->GetCurrentLOD() != -1 && chunkA->GetCurrentLOD() == chunkB->GetCurrentLOD()) {
                StitchChunkBorder(chunkA, chunkB, dirA);
            }
        }
    }
}
