#pragma once

#include <d3d11.h>
#include <vector>
#include <memory>

#include <Managers/RenderManager/RenderPass.h>
#include <Managers/ShaderManager.h>
#include <Managers/DeviceManager.h>
#include <Managers/RenderTargetManager.h>
#include <Managers/InitManager.h>
#include <Config/Base/RenderPass/ShadowPassConfig.h>

class Shadows;

class ShadowPass : public RenderPass
{
private:
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<ShaderManager> m_shaderManager;
    std::shared_ptr<RenderTargetManager> m_renderTargetManager;
    std::shared_ptr<InitManager> m_initManager;
    std::shared_ptr<Shadows> m_shadows;
    std::shared_ptr<ShadowPassConfig> config;
public:
    std::vector<std::shared_ptr<PipelineOperation>> BeginPass() override;
    std::vector<std::shared_ptr<PipelineOperation>> ExecPass(std::shared_ptr<MeshAsset> mesh) override;
    std::vector<std::shared_ptr<PipelineOperation>> EndPass() override;
    void SetInitialOperations();
    HRESULT Init() override;
    HRESULT InitManagers();
};