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

//class RenderManager; // Forward declaration

class ChunkService : public IService
{
private:    
	float m_chunkSize = 0.0f;
    int m_renderDistanceChunks = 0;

    //DirectX::XMFLOAT4 m_frustumPlanes[6];

    std::vector<std::shared_ptr<Chunk>> m_chunksToDelete;
    mutable std::mutex m_chunkDeletionMutex;
    ChunkMap m_chunks;

    WorldTerrain::TerrainChunkDefinition m_def;
	std::shared_ptr<ProceduralService> m_proceduralService;
    std::shared_ptr<DeviceManager> m_deviceManager;
    //std::shared_ptr<RenderManager> m_renderManager;
    std::shared_ptr<CameraManager> m_cameraManager;

    //void StitchChunks();
    std::vector<std::shared_ptr<Chunk>> GetNeighbors(const TerrainChunk::ChunkID& id);
public:
    ChunkService() {};
    ~ChunkService() override {};

    // Métodos de IService
    HRESULT Init() override { return S_OK; };
    HRESULT Init(WorldTerrain::TerrainChunkDefinition definition);
    void Update(float deltaTime) override {};
    void Render() override {};
    void Shutdown() override {};

	void EmptyRecycleBin();

    const bool IsDirty() const;

    // Implementación de IService
    const std::string& GetServiceName() const override {
        static const std::string name = "ChunkService";
        return name;
    }
    static const std::string& GetStaticServiceName() {
        static const std::string name = "ChunkService";
        return name;
    }
    
    void SetProceduralService(std::shared_ptr<ProceduralService> proceduralService) {
        m_proceduralService = proceduralService;
	}

	void UpdateChunks(DirectX::XMFLOAT3 worldPosition);
        
    // Métodos CRUD para la gestión de Chunks
    template<typename T, typename... Args>
    HRESULT LoadChunk(const TerrainChunk::ChunkID& id);
    std::shared_ptr<Chunk> GetChunk(const TerrainChunk::ChunkID& id) const;
    std::shared_ptr<Chunk> GetNeighbor(const TerrainChunk::ChunkID& id, TerrainChunk::NeighborDirection direction);
    HRESULT UnloadChunk(const TerrainChunk::ChunkID& id);
    ChunkMap GetChunks() const { return m_chunks; }
    std::vector<std::shared_ptr<Chunk>> GetChunksAsVector() const;
    // Debug
	int GetNumChunks() const { return static_cast<int>(m_chunks.size()); }
    /*
    HRESULT UpdateChunk(const TerrainChunk::ChunkID& id);
    const std::unordered_map<TerrainChunk::ChunkID, std::unique_ptr<IChunk>, ChunkHasher>& GetChunks() const;*/

    std::vector<std::shared_ptr<Chunk>> GetFrustumChunks(const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes, const DirectX::XMFLOAT3 cameraPosition);
};

// Implementación de la plantilla LoadChunk en el .h, ya que debe ser visible para el compilador
template<typename T, typename... Args>
HRESULT ChunkService::LoadChunk(const TerrainChunk::ChunkID& id)
{
    if (m_chunks.count(id)) {
        return S_OK;
    }

    auto newChunk = std::make_shared<T>(id);
    if (!newChunk) {
        return E_FAIL;
    }

	newChunk->SetProceduralEngine(m_proceduralService->GetEngine());
	newChunk->SetChunkSize(m_chunkSize);
    newChunk->Init();
    m_chunks[id] = std::move(newChunk);
    return S_OK;
}