#pragma once
#define NOMINMAX

// #include <Game/Systems/Terrain/Factory/IChunkFactory.h> // Removed - not needed in header
#include <Assets/Base/MeshAssetBase.h>
#include <Config/Game/System/TerrainConfig.h>
#include <DirectXMath.h>
#include <Game/Systems/Terrain/Factory/TerrainDistributionFactory.h>
#include <Game/Systems/Terrain/Factory/TerrainGeometryFactory.h>
#include <IService.h>
#include <Managers/CameraManager.h>
#include <memory>
#include <vector>
#include <windows.h>

class TerrainAsset;
class ProceduralService;

class Terrain : public IService {
public:
    Terrain();
    ~Terrain();

    HRESULT Init() override;
    HRESULT InitServices();
    HRESULT InitConfig();
    HRESULT InitMesh();
    void Update(float deltaTime) override;

    const std::string& GetServiceName() const override {
        static const std::string name = "Terrain";
        return name;
    }
    static const std::string& GetStaticServiceName()
    {
        static const std::string name = "Terrain";
        return name;
    }

    std::shared_ptr<TerrainConfig> GetConfig() const { return m_config; }
    // ChunkService removed - chunks now managed by TerrainDistributionFactory
    std::shared_ptr<ProceduralService> GetProceduralService() const { return m_proceduralService; }
    MeshAssetBase* GetTerrainMesh() { return nullptr; } // m_terrainMesh.get(); }
    std::vector<IChunk*> GetVisibleChunks();
    float GetTerrainHeight(float x, float z) const;

    void Generate();
    void Render();

    // Bind vertex/index and instance CBs before render (used by RenderManager pre-render)
    void BindForRender(ID3D11DeviceContext* context);

    bool IsDirty() const { return m_dirty; }
    bool IsGenerating() const { return m_isGenerating; }
    void SetGenerating(bool generating) { m_isGenerating = generating; }
    void SetDirty(bool dirty) { m_dirty = dirty; }

private:
    TerrainGeometryFactory* m_geometryFactory;
    TerrainDistributionFactory* m_terrainFactory;
    std::string m_terrainEngineName;

    std::shared_ptr<TerrainConfig> m_config;
    std::shared_ptr<CameraManager> m_cameraManager;
    // m_chunkService removed - chunks now managed directly by distribution engines
    std::shared_ptr<ProceduralService> m_proceduralService;
    TerrainAsset* m_terrainAsset = nullptr;

    float m_chunkSize = 0.0f;
    bool m_dirty = false;
    bool m_isGenerating = false;
    XMFLOAT3 m_lastCameraPosition;
};
