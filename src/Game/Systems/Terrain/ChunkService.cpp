#include "ChunkService.h"
#include <Assets/Base/TerrainAsset.h>
#include <Defines/Contants/Flags.h>
#include <Game/Systems/Terrain/Chunk/Chunk.h>
#include <Helpers/FrustumCullingHelper.h>
#include <Helpers/PointerQueryRaw.h>
#include <ManagerLocator/ManagerLocator.h>
#include <REGISTER_SERVICE_MACRO.h>

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
				int lodLevel = 5;
				/*if (distance >= m_chunkSize * 2.0f) {
					lodLevel = 4;
				}*/
				if (distance >= m_chunkSize * 30.0f) {
					lodLevel = 6;
				}

				/*if (distance > m_chunkSize * 2.0f) {
					lodLevel = 3;
				}
				if (distance > m_chunkSize * 3.0f) {
					lodLevel = 5;
				}
				if (distance > m_chunkSize * 4.0f) {
					lodLevel = 6;
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
					std::vector<Chunk*> neighbors = GetNeighbors(id);
					chunk->SetNeighbors(neighbors.data());
					chunk->UpdateLOD(lodLevel);
				}

			}
		}
	}

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

			// Se asume que el helper `IsAABBInFrustum` también ha sido refactorizado
			// para aceptar el `std::vector<Plane>`
			if (FrustumCullingHelper::IsAABBInFrustum(minCorner, maxCorner, frustumPlanes))
			{
				chunk->SetFlag(FLAG_CHUNK_VISIBLE, true);
				visibleChunks.push_back(chunk);
			}
		}
	}

	return visibleChunks;
}
