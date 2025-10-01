// PipelineResourcesManager.h
#pragma once
#include <Assets/Base/MeshAssetBase.h>
#include <Config/Base/Managers/EngineConfig.h>
#include <Config/Base/Managers/RenderManagerConfig.h>
#include <Config/Base/Pipeline/ShaderTextureSlotsConfig.h>
#include <Config/Base/RenderPass/BasePassConfig.h>
#include <d3d11.h>
#include <Defines/EngineDefinition.h>
#include <Defines/Structs/PipelineResources.h>
#include <Game/GameEngineConfig.h>
#include <ManagerBase.h>
#include <Managers/RenderManager/Pipeline/ConcreteResources.h>
#include <map>
#include <memory>
#include <PassConfigBase.h>
#include <Services/ServiceConfig.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>
#include <wrl/client.h>

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
    HRESULT InitManagers(EngineContext* context);
    HRESULT InitPasses(EngineContext* context);
    HRESULT InitMainPass();
    void InitConstantsBuffers(ID3D11DeviceContext* context, ShaderResource* shaderResource);
    void InitStencilView(DepthStencilResource& depthStencilResource, float width, float height);
    void InitRaserizerState(std::string stateName, D3D11_RASTERIZER_DESC& desc, Microsoft::WRL::ComPtr<ID3D11RasterizerState>& rasterizerState);
    void InitStencilState(std::string name, Microsoft::WRL::ComPtr<ID3D11DepthStencilState>& stencilState);
    std::vector<ID3D11SamplerState*> InitSamplers(std::unordered_map<std::string, ShaderSampler::SamplerDefinition> samplerDefinitions);

    void Update(float deltaTime) override {};
    void Shutdown() override;

    void LoadShader(const std::string& shaderName);
    ShaderResource* GetShader(const std::string& shaderName);

    void LoadMesh(const std::string& meshName);

    void SetMeshes(const std::vector<MeshAssetBase*> meshses);
    std::vector<MeshResource*> GetMeshes();

    //void UpdatePassesResources();

    void CreatePassResources(const std::string& passName);
    ShaderResource* CreateShaderResource(std::string shaderName);
    MeshResource* CreateMeshResource(MeshAssetBase* mesh);

    std::vector<RenderPassResource*> GetPasses() {
        std::vector<RenderPassResource*> passes;
        for (const auto& pass : m_passes) {
            passes.push_back(pass.second);
        }
        return passes;
    }

    PipelineMainInitialResources* GetInitialResources() const { return m_initialResources; }

    void SetShaderResource(std::string name, ShaderResource* shaderResource) {
        m_shaders[name] = shaderResource;
    }

    std::vector<ShaderResource*> GetShaders() {
        std::vector<ShaderResource*> result = {};
        for (auto& shader : m_shaders)
        {
            result.push_back(shader.second);
        }
        return result;
    }


private:

    std::map<std::string, MeshResource*> m_meshes{};
    std::map<std::string, ShaderResource*> m_shaders{};
    std::map<std::string, RenderPassResource*> m_passes{};
    PipelineMainInitialResources* m_initialResources{};

    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<ShaderManager> m_shaderManager;
    std::shared_ptr<ConcreteResources> m_concreteResources;

    // ---------------------------------------
    // Configuración
    // ---------------------------------------
    EngineConfig m_engineConfig{};
    GameEngineConfig m_gameConfig{};
    RenderManagerConfig m_renderConfig{};
    BasePassConfig m_passesBaseConfig{};
    ShaderTextureSlotsConfig m_shaderTextureSlotContract{};

    std::map<std::string, std::shared_ptr<PassConfigBase>> m_passesConfig{};
    ServiceConfig m_serviceConfig{};
};
