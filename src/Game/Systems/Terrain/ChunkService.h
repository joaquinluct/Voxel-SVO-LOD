#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>
#include "IService.h"
#include <DeviceManager.h>
#include <CameraManager.h>
#include "IChunk.h"
#include "Defines/TerrainChunk.h"
#include "Defines/WorldTerrain.h"
#include "ProceduralService.h"
#include <Game/Systems/Terrain/Chunk/Chunk.h>
#include <Defines/CameraDefinition.h>

class TerrainAsset; 

class ChunkService : public IService
{
private:    
	float m_chunkSize = 0.0f;
    int m_renderDistanceChunks = 0;

    //DirectX::XMFLOAT4 m_frustumPlanes[6];

    std::vector<Chunk*> m_chunksToDelete;
    mutable std::mutex m_chunkDeletionMutex;
	TerrainAsset* m_terrainAsset = nullptr;
    ChunkMap m_chunks;

    WorldTerrain::TerrainChunkDefinition m_def;
	std::shared_ptr<ProceduralService> m_proceduralService;
    std::shared_ptr<DeviceManager> m_deviceManager;
    //std::shared_ptr<RenderManager> m_renderManager;
    std::shared_ptr<CameraManager> m_cameraManager;

    //void StitchChunks();
    std::vector<Chunk*> GetNeighbors(const TerrainChunk::ChunkID& id);
public:
    ChunkService():
		m_deviceManager(nullptr), m_cameraManager(nullptr), m_proceduralService(nullptr), m_chunks(), m_chunksToDelete(),
		m_chunkSize(0.0f), m_renderDistanceChunks(0), m_def()
    {};
    ~ChunkService() override {};

    // Implementación de IService
    HRESULT Init() override { return S_OK; };
    HRESULT Init(WorldTerrain::TerrainChunkDefinition definition);
    void Update(float deltaTime) override {};
    void Render() override {};
    void Shutdown() override {};
    const std::string& GetServiceName() const override { static const std::string name = "ChunkService"; return name; }
    static const std::string& GetStaticServiceName() { static const std::string name = "ChunkService"; return name; }

	void SetTerrainAsset(TerrainAsset* terrainAsset) { m_terrainAsset = terrainAsset; } 

	// Establecer el servicio de generación procedural
    void SetProceduralService(std::shared_ptr<ProceduralService> proceduralService) { m_proceduralService = proceduralService; }

	// Control de estado
    const bool IsDirty() const;

    // Gestion de los chunks
    std::vector<Chunk*> GetVisibleChunks();
	std::vector<Chunk*> GetAllChunks();
    Chunk* GetChunk(const TerrainChunk::ChunkID& id) const;
    Chunk* GetNeighbor(const TerrainChunk::ChunkID& id, TerrainChunk::NeighborDirection direction);
    ChunkMap GetChunks() const { return m_chunks; }
    //template<typename T, typename... Args>
    HRESULT LoadChunk(const TerrainChunk::ChunkID& id);
    HRESULT UnloadChunk(const TerrainChunk::ChunkID& id);
	void EmptyRecycleBin();
	void UpdateChunks(DirectX::XMFLOAT3 worldPosition);
    std::vector<Chunk*> GetFrustumChunks(const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes, const DirectX::XMFLOAT3 cameraPosition);

    // Debug
	int GetNumChunks() const { return static_cast<int>(m_chunks.size()); }
    /*
    HRESULT UpdateChunk(const TerrainChunk::ChunkID& id);
    const std::unordered_map<TerrainChunk::ChunkID, std::unique_ptr<IChunk>, ChunkHasher>& GetChunks() const;*/

};

// Implementación de la plantilla LoadChunk en el .h, ya que debe ser visible para el compilador
//template<typename T, typename... Args>
//HRESULT ChunkService::LoadChunk(const TerrainChunk::ChunkID& id)
//{
//    if (m_chunks.count(id)) {
//        return S_OK;
//    }
//
//    T newChunk = T(id);
//    if (newChunk.) {
//        return E_FAIL;
//    }
//
//	newChunk->SetProceduralEngine(m_proceduralService->GetEngine());
//	newChunk->SetChunkSize(m_chunkSize);
//    newChunk->Init();
//    m_chunks[id] = &newChunk;
//    return S_OK;
//}