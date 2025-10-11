#pragma once

#include "TerrainEngineBase.h"
#include "TerrainGeometryEngineBase.h"
#include <Assets/Base/MeshAssetBase.h>
#include <Defines/CameraDefinition.h>
#include <DirectXMath.h>
#include <Game/System/TerrainConfig.h>
#include <Game/Systems/Terrain/Defines/Chunk/TerrainChunkConfig.h>
#include <iCamera.h>
#include <Locators/TerrainEngineLocator/TerrainEngineLocator.h>
#include <memory>
#include <string>
#include <vector>

class TerrainDistributionEngineBase : public TerrainEngineBase
{
protected:
    std::vector<MeshAssetBase*> m_meshes;

    TerrainGeometryEngineBase* m_geometryFactory;
    TerrainChunkConfig m_chunkConfig;

    bool m_isDirty;
    bool m_isGeneratingDistribution;

    DirectX::XMFLOAT3 m_lastWorldPosition;
public:
    void Init(TerrainConfig* config) override {
        std::string geometryEngineName = config->geometry_engine;
        if (geometryEngineName.empty()) {
            return;
        }
        std::shared_ptr<TerrainGeometryEngineBase> geometryEngine = TerrainEngineLocator::GetEngine<TerrainGeometryEngineBase>(geometryEngineName);
        m_geometryFactory = geometryEngine.get();
        m_geometryFactory->Init(config);
    };
    std::vector<MeshAssetBase*> GetVisibleMeshes(std::vector<CameraDefinition::FrustumPlane>& frustumPlanes, int zone = 0) override { return {}; }
    void UpdateTerrain(const DirectX::XMFLOAT3& worldPosition) override {};
    void Generate(ICamera* camera) override {};
    TerrainChunkConfig* GetChunkConfig() { return &m_chunkConfig; }

    bool IsDirty() { return m_isDirty; }
    void SetDirty(bool dirty) { m_isDirty = dirty; }

    bool IsGeneratingDistribution() { return m_isGeneratingDistribution; }
    void SetIsGeneratingDistribution(bool isGenerating) { m_isGeneratingDistribution = isGenerating; }
};
