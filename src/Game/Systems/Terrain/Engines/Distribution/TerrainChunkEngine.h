#pragma once

#include <Defines/CameraDefinition.h>
#include <Defines/TerrainChunk.h>
#include <DirectXMath.h>
#include <Game/Systems/Terrain/Defines/TerrainDistributionEngineBase.h>
#include <Game/Systems/Terrain/Defines/TerrainGeometryEngineBase.h>
#include <Game/Systems/Terrain/Factory/IChunkFactory.h>
#include <IChunk.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations
class TerrainConfig;
class TerrainChunkEngineConfig;
class ICamera;

/**
 * Engine central de distribución basado en chunks.
 * Coordina la gestión de chunks y los geometry engines para el renderizado del terreno.
 * Se configura mediante YAML y utiliza TerrainEngineLocator para resolver dependencias.
 */
class TerrainChunkEngine : public TerrainDistributionEngineBase {
public:
    TerrainChunkEngine();
    ~TerrainChunkEngine();

    // Implementación de TerrainDistributionEngineBase
    void Init(TerrainConfig* config) override;
    std::vector<MeshAssetBase*> GetVisibleMeshes(std::vector<CameraDefinition::FrustumPlane>& frustumPlanes, int zone = 0) override;
    void UpdateTerrain(const DirectX::XMFLOAT3& worldPosition) override;
    void Generate(ICamera* camera) override;

    // Métodos específicos del engine de chunks
    void Render(ICamera* camera);
    void SetChunkFactory(std::shared_ptr<IChunkFactory> factory) { m_chunkFactory = factory; }
    void SetGeometryEngine(TerrainGeometryEngineBase* engine) { m_geometryEngine = engine; }

    // Getters para inspección
    size_t GetActiveChunkCount() const { return m_activeChunks.size(); }
    bool HasChunk(const TerrainChunk::ChunkID& id) const { return m_activeChunks.count(id) > 0; }

    static constexpr const char* engineName = "TerrainChunkEngine";

private:
    // Configuraciones
    TerrainConfig* m_terrainConfig = nullptr;
    TerrainChunkEngineConfig* m_engineConfig = nullptr;

    // Parámetros de configuración extraídos del YAML
    float m_chunkSize = 128.0f;
    int m_chunkRadius = 8;
    float m_worldWidth = 163840.0f;
    float m_worldHeight = 163840.0f;
    float m_terrainHeightMult = 3.164f;
    float m_oceanLevel = -2.0f;
    uint32_t m_seed = 1234567890;

    // Factories y engines
    std::shared_ptr<IChunkFactory> m_chunkFactory;
    TerrainGeometryEngineBase* m_geometryEngine = nullptr;

    // Gestión de chunks
    using ChunkMap = std::unordered_map<TerrainChunk::ChunkID, std::unique_ptr<IChunk>, TerrainChunk::ChunkHasher>;
    ChunkMap m_activeChunks;
    std::vector<std::unique_ptr<IChunk>> m_chunksToDelete; // Cola de eliminación diferida

    // Estado del sistema
    DirectX::XMFLOAT3 m_lastPlayerPosition = { 0.0f, 0.0f, 0.0f };
    bool m_isDirty = true;
    bool m_isUpdating = false;

    // Métodos privados
    void InitializeFromConfig();
    void LoadChunkFactory(const std::string& chunkType);
    void LoadGeometryEngine(const std::string& geometryEngine);

    void LoadChunksInRadius(const DirectX::XMFLOAT3& centerPosition);
    void UnloadChunksOutsideRadius(const DirectX::XMFLOAT3& centerPosition);
    HRESULT LoadSingleChunk(const TerrainChunk::ChunkID& id);

    std::vector<TerrainChunk::ChunkID> GetVisibleChunkIds(const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes, int zone = 0);
    bool IsChunkInFrustum(const TerrainChunk::ChunkID& id, const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes) const;
    float CalculateDistanceToChunk(const TerrainChunk::ChunkID& id, const DirectX::XMFLOAT3& position) const;

    // Utilidades
    DirectX::XMFLOAT3 GetChunkWorldPosition(const TerrainChunk::ChunkID& id) const;
    TerrainChunk::ChunkID WorldPositionToChunkID(const DirectX::XMFLOAT3& worldPos) const;

    void CleanupDeletedChunks();
};
