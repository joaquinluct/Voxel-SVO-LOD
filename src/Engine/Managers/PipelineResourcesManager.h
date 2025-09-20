// PipelineResourcesManager.h
#pragma once
#include <Defines/EngineDefinition.h>
#include <ManagerBase.h>
#include <memory>
#include <string>
#include <Windows.h>

class DeviceManager;
class ShaderManager;

class PipelineResourcesManager : public ManagerBase
{
public:
    PipelineResourcesManager();
    ~PipelineResourcesManager() override;
    const std::string& GetManagerName() const override {
        static const std::string name = "PipelineResourcesManager";
        return name;
    }
    static const std::string& GetStaticManagerName()
    {
        static const std::string name = "PipelineResourcesManager";
        return name;
    }

    HRESULT Init(EngineContext* context) override;
    void Update(float deltaTime) override {};
    void Shutdown() override;

    void LoadShader(const std::string& shaderName);
    void LoadMesh(const std::string& meshName);

private:
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<ShaderManager> m_shaderManager;
};
