#pragma once

#include <d3d11.h>
#include <Defines/Enums/Matrix.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/ShaderSampler.h>
#include <GUID.h>
#include <map>
#include <string>
#include <string_view>
#include <Threading/SyncFlagged.h>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

class MeshAssetBase;
class ShaderAsset;

struct PipelineResourceBase
{
    std::string id;
    std::string name;
    bool isLoaded = false;
};

struct CurrentPipelineStateResource : public PipelineResourceBase
{
    std::string_view currentShaderName;
    std::string currentRenderTargetViewName;
    std::string currentViewportName;

    bool isCullModeEnabled = false;
    bool isDepthTestEnabled = false;
    bool isDepthWriteEnabled = false;
    bool isStencilTestEnabled = false;
    bool isBlendModeEnabled = false;
    bool isWireframeEnabled = false;
};

struct DepthStencilResource : public PipelineResourceBase
{
    std::string name;
    Microsoft::WRL::ComPtr <ID3D11DepthStencilView> stencilViewData;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewData;
    ID3D11Texture2D* viewTextureData = nullptr;
    D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDesc;
    D3D11_TEXTURE2D_DESC desc;
    UINT clearFlags;
    FLOAT depth;
    UINT8 stencil;
    bool hasViewDesc = false;
};

struct PipelineMainInitialResources : public PipelineResourceBase
{
    std::string depthStencilName;
    std::string viewportName;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    D3D11_RASTERIZER_DESC* rasterizerDesc;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizedState;
    DepthStencilResource* depthStencilResource;
    D3D11_VIEWPORT viewport;
    float clearColor[4];
    bool isLoaded;
};

struct PipelineConstantBufferResource : public PipelineResourceBase
{
    std::string name;
    bool updated = false;
    std::string matrixType;
    MatrixBufferTypeEnum bufferType; // Static o Dynamic
    unsigned int slot;
    MatrixDefinition::AnyMatrixBuffer data;
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};

struct RenderPassResource : public PipelineResourceBase
{
    int id;
    bool enabled = false;
    bool hasShader = false;
    bool hasDepthStencil = false;
    bool isRasterizerStateEnabled = true;
    bool isCullModeEnabled = false;
    bool isDepthTestEnabled = false;
    bool isDepthWriteEnabled = false;
    bool isStencilTestEnabled = false;
    bool isBlendModeEnabled = false;
    bool isWireframeEnabled = false;

    std::string_view shaderName;

    DepthStencilResource* depthStencilResource = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    D3D11_RASTERIZER_DESC rasterizedDesc;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    D3D11_BLEND_DESC blendDesc;
};


struct MeshResource : public PipelineResourceBase
{
    MeshAssetBase* mesh = nullptr;
    //std::vector<std::string> textures{};
    std::vector<std::string> passes{};
    std::vector<ID3D11ShaderResourceView*> textures{};
};

struct ShaderResource : public PipelineResourceBase
{
    ShaderResource() :
        samplerDefinitions{}, samplers{}
    {
        id = generateUID();
    };
    std::string id;
    ShaderAsset* shader = nullptr;
    SyncFlagged flags = {};
    bool initialOperationsExecuted = false;
    std::map<std::string, PipelineConstantBufferResource> constantBuffers;
    std::vector<ID3D11SamplerState*> samplers;
    std::unordered_map<std::string, ShaderSampler::SamplerDefinition> samplerDefinitions;
    ID3D11Buffer* GetConstantBufferByName(const std::string& name) const {
        auto it = constantBuffers.find(name);
        if (it != constantBuffers.end() && it->second.buffer) {
            return it->second.buffer.Get();
        }
        return nullptr;
    }

    std::vector<PipelineConstantBufferResource> GetConstantsBuffers() const {
        std::vector<PipelineConstantBufferResource> buffers{};
        for (const auto& [name, cb] : constantBuffers) {
            buffers.push_back(cb);
        }
        return buffers;
    }

    std::vector<ID3D11Buffer*> GetConstantBufferResources() const {
        std::vector<ID3D11Buffer*> buffers{};
        for (const auto& [name, cb] : constantBuffers) {
            if (cb.buffer) {
                buffers.push_back(cb.buffer.Get());
            }
        }
        return buffers;
    }

};
