#pragma once
#define NOMINMAX
#include <d3d11.h>
#include <vector>
#include <memory>
#include <Defines/Pass.h>
#include <Managers/ShaderManager.h>
#include <Managers/CameraManager.h>
#include <Managers/DeviceManager.h>
#include <Managers/InitManager.h>
//#include <Managers/RenderTargetManager.h>
#include <Managers/RenderManager/RenderPass.h>
#include <SceneManager.h>
#include <Config/Base/RenderPass/WaterPassConfig.h>

class World;
class Shadows;
class Lighting;
class MeshAsset;
class TerrainAsset;
class Terrain;
class FrameStateService;
class Water;
class WaterPassConfig;

class WaterPass : public RenderPass
{
private:
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<ShaderManager> m_shaderManager;
    //std::shared_ptr<RenderTargetManager> m_renderTargetManager;
	std::shared_ptr<TerrainAsset> m_terrainAsset;
    std::shared_ptr<InitManager> m_initManager;
	std::shared_ptr<FrameStateService> m_FrameStateService;

    std::shared_ptr<Lighting> m_lighthing;
    std::shared_ptr<Shadows> m_shadows;
    std::shared_ptr<World> m_world;
    std::shared_ptr<Water> m_water;
    std::shared_ptr<Terrain> m_terrain;

    std::shared_ptr<WaterPassConfig> config;
    std::wstring shaderName;

public:
	HRESULT Init() override;
    HRESULT InitManagers();
    
    std::vector<PipelineOperationType> BeginPass(const MeshAsset* mesh, FrameStateService* renderState) override;
    std::vector<PipelineOperationType> ExecPass(const MeshAsset* mesh, FrameStateService* renderState) override;
    std::vector<PipelineOperationType> EndPass() override;

    std::map<std::string, std::shared_ptr<MeshAsset>> GetMeshes(const SceneManager* SceneManager, FrameStateService* renderState) override;

    std::shared_ptr<PassConfigBase> GetConfig() const override { return config; };

    void SetInitialOperations(std::shared_ptr<MeshAsset> mesh, const std::shared_ptr<FrameStateService> renderState);
};