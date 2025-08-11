#pragma once

#include <d3d11.h>
#include <vector>
#include <memory>
#include <Managers/ShaderManager.h>
#include <Managers/CameraManager.h>
#include <Managers/DeviceManager.h>
#include <Managers/InitManager.h>
#include <Managers/RenderTargetManager.h>
#include <Managers/RenderManager/RenderPass.h>
#include <Managers/RenderManager/GameRenderManager.h>
#include <Config/Base/RenderPass/MainColorPassConfig.h>

class Shadows;
class Lighting;
class MeshAsset;

class MainColorPass: public RenderPass
{
private:
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<ShaderManager> m_shaderManager;
    std::shared_ptr<RenderTargetManager> m_renderTargetManager;
    std::shared_ptr<InitManager> m_initManager;

    std::shared_ptr<Lighting> m_lighthing;
    std::shared_ptr<Shadows> m_shadows;

    std::shared_ptr<MainColorPassConfig> config;

    std::wstring shaderName;
public:
    HRESULT Init() override;
    HRESULT InitManagers();

    std::vector<std::shared_ptr<PipelineOperation>> BeginPass() override;
    std::vector<std::shared_ptr<PipelineOperation>> ExecPass(std::shared_ptr<MeshAsset> mesh) override;
    std::vector<std::shared_ptr<PipelineOperation>> EndPass() override;

    std::map<std::string, std::shared_ptr<MeshAsset>> GetMeshes(GameRenderManager* gameRenderManager) override;

    void SetInitialOperations();
};