#include "PipelineResourcesManager.h"
#include <Assets/Base/MeshAssetBase.h>
#include <Assets/Base/ShaderAsset.h>
#include <d3d11.h>
#include <Defines/Contants/Flags/ShaderResources.h>
#include <Defines/EngineDefinition.h>
#include <Defines/Enums/Matrix.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/Matrix/MatrixDefinitionBase.h>
#include <Defines/ShaderSampler.h>
#include <Defines/Structs/PipelineResources.h>
#include <IPipelineRasterizedState.h>
#include <IPipelineShaderViewState.h>
#include <IPipelineStencilState.h>
#include <IPipelineStencilView.h>
#include <IPipelineViewportState.h>
#include <Locators/ConfigLocator/ConfigLocator.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <Locators/Pipeline/PipelineStateLocator.h>
#include <Managers/ManagerBase.h>
#include <Managers/RenderManager/Pipeline/ConcreteOperations.h>
#include <Managers/RenderManager/Pipeline/ConcreteResources.h>
#include <Managers/ShaderManager.h>
#include <map>
#include <memory>
#include <PassConfigBase.h>
#include <REGISTER_MANAGER_MACRO.h>
#include <string>
#include <unordered_map>
#include <Util/Text/Text.h>
#include <variant>
#include <vector>
#include <Windows.h>

REGISTER_MANAGER_TYPE(PipelineResourcesManager, "PipelineResourcesManager")

// ----------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------
PipelineResourcesManager::PipelineResourcesManager() : m_deviceManager(nullptr), m_shaders{} {
}

// ----------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------
PipelineResourcesManager::~PipelineResourcesManager()
{
    Shutdown();
}

// ----------------------------------------------------------------
// Inicialización de los Managers necesarios
// ----------------------------------------------------------------
HRESULT PipelineResourcesManager::InitManagers(EngineContext* context)
{
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (!m_deviceManager) {
        return E_FAIL;
    }
    m_shaderManager = ManagerLocator::GetManager<ShaderManager>();
    if (!m_shaderManager) {
        return E_FAIL;
    }

    return S_OK;
}

void PipelineResourcesManager::InitConstantsBuffers(ID3D11DeviceContext* context, ShaderResource* shaderResource) {
    /*if (shaderResource == nullptr || shaderResource->flags.GetFlag(FLAG_SHADER_HAS_CONSTANTS_BUFFERS_BINDED)) {
        return;
    }*/
    std::map<std::string, std::shared_ptr<MatrixDefinition::AnyMatrixBuffer>> buffers = shaderResource->shader->GetConstantsBuffers();
    for (const auto& [name, buffer] : buffers) {
        std::string matrixType = "";
        MatrixBufferTypeEnum bufferType = MatrixBufferTypeEnum::Dynamic;
        PipelineConstantBufferResource bufferResource{};
        std::visit([&](auto currentMatrix) {
            size_t matrixSize = currentMatrix.Size();
            matrixType = currentMatrix.MatrixType();
            bufferType = currentMatrix.BufferType();
            UINT slot = currentMatrix.Slot();
            m_concreteResources->CreateConstantBuffer(m_deviceManager->GetDevice(), buffer, name, matrixType, bufferType, static_cast<UINT>(matrixSize), slot, bufferResource);
            ID3D11Buffer* cBuffer = bufferResource.buffer.Get();
            if (matrixType == MATRIX_TYPE_VERTEX) {
                context->VSSetConstantBuffers(slot, 1, &cBuffer);
            }
            else {
                context->PSSetConstantBuffers(slot, 1, &cBuffer);
            }
            //bufferResource.buffer = cBuffer;
            }, *buffer);
        shaderResource->constantBuffers[name] = bufferResource;
    }
    shaderResource->flags.SetFlag(FLAG_SHADER_HAS_CONSTANTS_BUFFERS_BINDED, true);
}

void PipelineResourcesManager::InitRaserizerState(std::string stateName, D3D11_RASTERIZER_DESC& desc, Microsoft::WRL::ComPtr<ID3D11RasterizerState>& rasterizerState)
{
    // RASTERIZER STATE

    std::shared_ptr<IPipelineRasterizedState> state = PipelineStateLocator::GetPipelineState<IPipelineRasterizedState>(stateName);

    desc.FillMode = static_cast<D3D11_FILL_MODE>(state->FillMode);
    desc.CullMode = static_cast<D3D11_CULL_MODE>(state->CullMode);
    desc.FrontCounterClockwise = static_cast<BOOL>(state->FrontCounterClockwise);
    desc.AntialiasedLineEnable = static_cast<BOOL>(state->AntialiasedLineEnable);
    desc.DepthClipEnable = static_cast<BOOL>(state->DepthClipEnable);

    /*PipelineRasteriezeData m_param = {};
    m_param.desc = desc;
    m_param.name = stateName;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> data = nullptr;*/

    HRESULT hr = m_deviceManager->GetDevice()->CreateRasterizerState(&desc, &rasterizerState);
}

void PipelineResourcesManager::InitStencilState(std::string name, Microsoft::WRL::ComPtr<ID3D11DepthStencilState>& stencilState) {

    std::shared_ptr<IPipelineStencilState> state = PipelineStateLocator::GetPipelineState<IPipelineStencilState>(name);
    D3D11_DEPTH_STENCIL_DESC desc = {};

    desc.DepthEnable = static_cast<BOOL>(state->DepthEnable);
    desc.DepthWriteMask = static_cast<D3D11_DEPTH_WRITE_MASK>(state->DepthWriteMask);
    desc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(state->DepthFunc);
    desc.StencilEnable = static_cast<BOOL>(state->StencilEnable);
    desc.StencilReadMask = static_cast<UINT8>(state->StencilReadMask);
    desc.StencilWriteMask = static_cast<UINT8>(state->StencilWriteMask);

    HRESULT hr = m_deviceManager->GetDevice()->CreateDepthStencilState(&desc, &stencilState);
}

std::vector<ID3D11SamplerState*> PipelineResourcesManager::InitSamplers(std::unordered_map<std::string, ShaderSampler::SamplerDefinition> samplerDefinitions) {

    std::vector<ID3D11SamplerState*> samplers;
    for (const auto& pair : samplerDefinitions) {
        const std::string& name = pair.first;
        const ShaderSampler::SamplerDefinition& samplerDef = pair.second;
        ID3D11SamplerState* samplerState = nullptr;
        HRESULT hr = m_deviceManager->GetDevice()->CreateSamplerState(&samplerDef.desc, &samplerState);
        if (SUCCEEDED(hr)) {
            samplers.push_back(samplerState);
        }
    }
    return samplers;
}

void PipelineResourcesManager::InitStencilView(DepthStencilResource& depthStencilResource, float width, float height) {
    std::shared_ptr<IPipelineStencilView> state = PipelineStateLocator::GetPipelineState<IPipelineStencilView>(depthStencilResource.name);

    D3D11_TEXTURE2D_DESC desc = {};

    desc.ArraySize = static_cast<UINT>(state->ArraySize);
    desc.BindFlags = static_cast<UINT>(state->BindFlags);
    desc.Format = static_cast<DXGI_FORMAT>(state->Format);
    desc.MipLevels = static_cast<UINT>(state->MipLevels);
    desc.SampleDesc.Count = static_cast<UINT>(state->SampleCount);
    desc.SampleDesc.Quality = static_cast<UINT>(state->SampleQuality);
    desc.Usage = static_cast<D3D11_USAGE>(state->Usage);
    desc.Width = static_cast<UINT>(state->Width > 0 ? state->Width : width);
    desc.Height = static_cast<UINT>(state->Height > 0 ? state->Height : height);

    depthStencilResource.hasViewDesc = state->ViewDesc;

    if (state->ViewDesc) {
        depthStencilResource.viewDesc = {};
        depthStencilResource.viewDesc.Format = static_cast<DXGI_FORMAT>(state->ViewFormat);
        depthStencilResource.viewDesc.ViewDimension = static_cast<D3D11_DSV_DIMENSION>(state->ViewDimension);
        depthStencilResource.viewDesc.Texture2D.MipSlice = static_cast<UINT>(state->ViewMipSlice);
    }

    if (state->ShaderView.size()) {
        std::shared_ptr<IPipelineShaderViewState> shaderViewState = PipelineStateLocator::GetPipelineState<IPipelineShaderViewState>(state->ShaderView);

        if (shaderViewState != nullptr) {
            depthStencilResource.shaderViewDesc = {};
            depthStencilResource.shaderViewDesc.Format = static_cast<DXGI_FORMAT>(shaderViewState->Format);
            depthStencilResource.shaderViewDesc.ViewDimension = static_cast<D3D11_SRV_DIMENSION>(shaderViewState->ViewDimension);
            depthStencilResource.shaderViewDesc.Texture2D.MipLevels = static_cast<UINT>(shaderViewState->MipLevels);
            depthStencilResource.shaderViewDesc.Texture2D.MostDetailedMip = static_cast<UINT>(shaderViewState->MostDetailedMip);
        }
    }
    depthStencilResource.desc = desc;
    //depthStencilResource.name = stateName;
    depthStencilResource.depth = static_cast<FLOAT>(state->Depth);
    depthStencilResource.stencil = static_cast<UINT8>(state->Stencil);
}

HRESULT PipelineResourcesManager::InitMainPass() {

    if (m_initialResources && m_initialResources->isLoaded) {
        return S_OK;
    }

    m_initialResources = new PipelineMainInitialResources();
    m_concreteResources = std::make_shared<ConcreteResources>();

    // BackBuffer
    m_concreteResources->CreateBackBuffer(m_deviceManager->GetSwapChain(), m_initialResources->backBuffer);
    if (!m_initialResources->backBuffer) {
        return E_FAIL;
    }

    // ViewPort
    std::string viewPortName = m_passesBaseConfig.viewPortState;
    std::shared_ptr<IPipelineViewportState> state = PipelineStateLocator::GetPipelineState<IPipelineViewportState>(viewPortName);
    state->Width = static_cast<float>(m_deviceManager->GetWidth());
    state->Height = static_cast<float>(m_deviceManager->GetHeight());
    D3D11_VIEWPORT viewport;
    m_concreteResources->CreateViewPort(m_deviceManager, state, viewport);

    m_initialResources->viewport = viewport;

    // Render Target View
    m_concreteResources->CreateRenderTargetView(m_deviceManager, m_initialResources->backBuffer, m_initialResources->renderTargetView);
    if (!m_initialResources->renderTargetView) {
        return E_FAIL;
    }

    // Depth Stencil View
    m_initialResources->depthStencilResource = new DepthStencilResource();
    m_initialResources->depthStencilResource->name = m_passesBaseConfig.StencilView;
    m_initialResources->depthStencilName = m_passesBaseConfig.StencilView;
    m_initialResources->viewportName = viewPortName;

    InitStencilView(*m_initialResources->depthStencilResource, m_deviceManager->GetWidth(), m_deviceManager->GetHeight());

    m_concreteResources->InitDepthSencilView(m_deviceManager->GetDevice(), m_initialResources->depthStencilResource);

    // Clear Color
    m_initialResources->clearColor[0] = 0.0f;
    m_initialResources->clearColor[1] = 0.2f;
    m_initialResources->clearColor[2] = 0.4f;
    m_initialResources->clearColor[3] = 1.0f;

    // Rasterizer State
    //m_concreteResources->CreateRasterizerState(m_deviceManager->GetDevice(), m_initialResources->rasterizerDesc, m_initialResources->rasterizedState);

    m_initialResources->isLoaded = true;

    return S_OK;
}

void PipelineResourcesManager::CreatePassResources(const std::string& passName) {
    const auto& it = m_passes.find(passName);
    if (it != m_passes.end()) {
        RenderPassResource* passResource = it->second;
        const auto& configIt = m_passesConfig.find(passName);
        //const auto& configIt = ConfigLocator::GetConfig<PassConfigBase>(passName + "Config");
        if (configIt != m_passesConfig.end()) {
            PassConfigBase* passConfig = configIt->second.get();
            passResource->isBlendModeEnabled = !passConfig->blendState.empty();
            passResource->isCullModeEnabled = !passConfig->rasterizedState.empty();
            passResource->isDepthTestEnabled = !passConfig->stencilState.empty();
            passResource->shaderName = passConfig->shader_name.empty() ? passConfig->shader_default : passConfig->shader_name;
            passResource->hasShader = false;

            // Shader
            std::string shaderName = passResource->shaderName.data();
            if (shaderName.size() > 0) {
                const auto& shaderIt = m_shaders.find(shaderName);
                if (shaderIt == m_shaders.end() && shaderName.size()) {
                    std::shared_ptr<ShaderAsset> shader = m_shaderManager->LoadShaderByName(StringToWstring(shaderName));
                    if (shader != nullptr) {
                        ShaderResource* shaderResource = new ShaderResource();
                        shaderResource->shader = shader.get();
                        shaderResource->id = shader->GetID();
                        shaderResource->name = shaderName;
                        shaderResource->samplerDefinitions = shader->GetSamplersDesc();
                        shaderResource->samplers = InitSamplers(shaderResource->samplerDefinitions);
                        //if (!shaderResource->flags.GetFlag(FLAG_SHADER_HAS_CONSTANTS_BUFFERS_DEFINED)) {
                        InitConstantsBuffers(m_deviceManager->GetContext().Get(), shaderResource);
                        //}

                        m_shaders[shaderName] = shaderResource;
                        //passResource->shaderName = shaderName;
                        passResource->hasShader = true;
                    }
                }
                else if (shaderIt != m_shaders.end()) {
                    //passResource->shaderName = shaderName;
                    passResource->hasShader = true;
                }
            }

            // Stencil state
            passResource->hasDepthStencil = passConfig->stencilState.size();
            if (passResource->hasDepthStencil) {
                passResource->depthStencilResource = new DepthStencilResource();
                passResource->depthStencilResource->name = passConfig->stencilState;
                const auto& state = PipelineStateLocator::GetPipelineState<IPipelineStencilView>(passConfig->stencilState);
                if (state != nullptr) {
                    InitStencilView(*passResource->depthStencilResource, m_deviceManager->GetWidth(), m_deviceManager->GetHeight());
                    m_concreteResources->InitDepthSencilView(m_deviceManager->GetDevice(), passResource->depthStencilResource);
                }
            }

            // Rasterizer state
            passResource->isRasterizerStateEnabled = passConfig->rasterizedState.size();
            if (passResource->isRasterizerStateEnabled) {
                InitRaserizerState(passConfig->rasterizedState, passResource->rasterizedDesc, passResource->rasterizerState);
            }

            // Stencil state
            passResource->hasDepthStencil = passConfig->stencilDef.size() && StrToLower(passConfig->stencilDef) != "none";
            if (passResource->hasDepthStencil) {
                InitStencilState(passConfig->stencilDef, passResource->depthStencilState);
            }
        }
    }
}

// ----------------------------------------------------------------
// Inicialización de los Passes de Renderizado
// ----------------------------------------------------------------
HRESULT PipelineResourcesManager::InitPasses(EngineContext* context)
{
    HRESULT hr = InitMainPass();
    if (FAILED(hr)) {
        return hr;
    }

    const std::vector<std::string>& allPasses = m_passesBaseConfig.passes;

    for (const auto& passName : allPasses) {
        std::string passConfigName = passName + "Config";
        auto config = ConfigLocator::GetConfig<PassConfigBase>(passConfigName);
        config->enabled = false;
        m_passesConfig[passName] = config;
        RenderPassResource* passResource = new RenderPassResource();
        passResource->name = passName;
        passResource->id = config->id;
        passResource->shaderName = config->shader_name.empty() ? config->shader_default : config->shader_name;
        passResource->isBlendModeEnabled = !config->blendState.empty();
        passResource->isCullModeEnabled = !config->rasterizedState.empty();
        passResource->isDepthTestEnabled = !config->stencilState.empty();
        m_passes[passName] = passResource;
    }

    const std::vector<std::string>& passesConfig = m_renderConfig.passes;
    for (const auto& passName : passesConfig) {
        const auto& it = m_passesConfig.find(passName);
        if (it != m_passesConfig.end()) {
            m_passesConfig[passName]->enabled = true;
            m_passes[passName]->enabled = true;
            CreatePassResources(passName);
        }
    }

    return S_OK;
}

// ----------------------------------------------------------------
// Inicialización general
// ----------------------------------------------------------------
HRESULT PipelineResourcesManager::Init(EngineContext* context)
{
    ManagerBase::Init(context);

    m_shaders.clear();
    m_meshes.clear();

    HRESULT hr = InitManagers(context);
    if (FAILED(hr)) {
        return hr;
    }

    hr = InitPasses(context);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

//void PipelineResourcesManager::UpdatePassesResources() {
//    for (const auto& passPair : m_passesConfig) {
//        const auto& passConfig = passPair.second;
//        const std::string& passName = passPair.first;
//        const auto& it = m_passes.find(passName);
//        if (passConfig != nullptr && it == m_passes.end()) {
//            RenderPassResource* passResource = new RenderPassResource();
//            passResource->id = passConfig->id;
//            passResource->name = passName;
//            std::string shaderName = passConfig->shader_name.empty() ? passConfig->shader_default : passConfig->shader_name;
//            passResource->shaderName = shaderName;
//            passResource->isCullModeEnabled = passConfig->rasterizedState.size() > 0;
//            passResource->isDepthTestEnabled = passConfig->stencilState.size() > 0;
//            passResource->isBlendModeEnabled = passConfig->blendState.size() > 0;
//            m_passes[passName] = passResource;
//
//        }
//    }
//}

ShaderResource* PipelineResourcesManager::CreateShaderResource(std::string shaderName) {
    //const auto& it = m_shaders.find(shaderName);
    /*if (it != m_shaders.end()) {
        return it->second;
    }*/

    std::shared_ptr<ShaderAsset> shader = m_shaderManager->LoadShaderByName(StringToWstring(shaderName));
    if (shader != nullptr) {
        ShaderResource* shaderResource = new ShaderResource();
        shaderResource->flags.SetFlag(FLAG_SHADER_HAS_CONSTANTS_BUFFERS_DEFINED, false);
        shaderResource->flags.SetFlag(FLAG_SHADER_HAS_CONSTANTS_BUFFERS_BINDED, false);
        shaderResource->shader = shader.get();
        shaderResource->id = shader->GetID();
        shaderResource->name = shaderName;

        // Samplers
        shaderResource->samplerDefinitions = shader->GetSamplersDesc();
        shaderResource->samplers = InitSamplers(shaderResource->samplerDefinitions);

        // Constant Buffers
        std::map<std::string, std::shared_ptr<MatrixDefinition::AnyMatrixBuffer>> buffers = shader->GetConstantsBuffers();
        for (const auto& [name, buffer] : buffers) {
            unsigned int vertexSize = 0;
            unsigned int slot = 0;
            std::string matrixType = "";
            MatrixBufferTypeEnum bufferType = MatrixBufferTypeEnum::Dynamic;
            PipelineConstantBufferResource bufferResource{};
            std::visit([&](auto& currentVertex) {
                vertexSize = currentVertex.Size();
                matrixType = currentVertex.MatrixType();
                bufferType = currentVertex.BufferType();
                slot = currentVertex.Slot();
                m_concreteResources->CreateConstantBuffer(m_deviceManager->GetDevice(), buffer, name, matrixType, bufferType, vertexSize, slot, bufferResource);
                }, *buffer);
            bufferResource.slot = slot;
            shaderResource->constantBuffers[name] = bufferResource;
            slot++;
        }

        shaderResource->flags.SetFlag(FLAG_SHADER_HAS_CONSTANTS_BUFFERS_DEFINED, true);
        m_shaders[shaderName] = shaderResource;
        return shaderResource;
    }
    return nullptr;
}

MeshResource* PipelineResourcesManager::CreateMeshResource(MeshAssetBase* mesh) {
    MeshResource* meshResource = new MeshResource();
    meshResource->mesh = mesh;
    const std::string& shaderName = mesh->GetShaderName();
    if (!shaderName.size()) return nullptr;

    /*ShaderResource* shaderResource = GetShader(shaderName);

    if (!shaderResource) {
        shaderResource = CreateShaderResource(shaderName);
    }*/

    //if (!shaderResource) return nullptr;

    //ShaderAsset* shader = shaderResource->shader;
    std::vector<ID3D11ShaderResourceView*> textures = mesh->GetMaterial()->GetTextures();

    if (textures.size()) {
        meshResource->textures = textures;
    }

    return meshResource;
}

void PipelineResourcesManager::SetMeshes(const std::vector<MeshAssetBase*> meshses) {
    for (auto mesh : meshses) {
        if (mesh) {
            std::string id = mesh->GetID();
            const auto& it = m_meshes.find(id);
            if (it == m_meshes.end()) {
                m_meshes[id] = CreateMeshResource(mesh);
            }
        }
    }
}
std::vector<MeshResource*> PipelineResourcesManager::GetMeshes() {
    std::vector<MeshResource*> result;
    for (const auto& pair : m_meshes) {
        result.push_back(pair.second);
    }
    return result;
}


void PipelineResourcesManager::LoadShader(const std::string& shaderName) {
    m_shaderManager->LoadShaderByName(StringToWstring(shaderName));
}

ShaderResource* PipelineResourcesManager::GetShader(const std::string& shaderName) {
    const auto& it = m_shaders.find(shaderName);
    if (it != m_shaders.end()) {
        return it->second;
    }
    return nullptr;
}

void PipelineResourcesManager::LoadMesh(const std::string& meshName) {
    //m_shaderManager->UnloadShaderByName(StringToWstring(shaderName)
    // );
}

// ----------------------------------------------------------------
// Shutdown
// ----------------------------------------------------------------
void PipelineResourcesManager::Shutdown()
{

}
