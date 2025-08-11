#pragma once
#include <Windows.h>
#include <IManager.h>
#include <IWindowDependentInitializable.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <variant>

#include <RenderManager/Pipeline/RenderPipelineExecutor.h>
#include <InitManager/Pipeline/PipelineConfigurator.h>
#include <RenderManager/RenderPass.h> // Incluimos la clase RenderPass para usar su forma de añadir operaciones
#include "Defines/Pipeline.h" // Incluimos las definiciones de operaciones y parámetros
#include "DeviceManager.h"
#include "ShaderManager.h"

#include <Config/Base/RenderStates/RasterizedMainColorPass.h>
#include <Config/Base/RenderStates/RasterizedShadowPass.h>
#include <Config/Base/RenderStates/StencilViewMainColor.h>
#include <Config/Base/RenderStates/BlendingUI.h>
#include <Config/Base/InitManagerConfig.h>

#include <Util/Text/Text.h>

class InitManager : public IManager, public IWindowDependentInitializable
{
private:
    MatrixDefinitionBase::MatrixParams m_constantsBuffers;

    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<ShaderManager> m_shaderManager;

    std::shared_ptr<InitManagerConfig> m_config;
	
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

    // Instancia del executor del pipeline
    //std::unique_ptr<RenderPipeline::RenderPipelineExecutor> m_pipelineExecutor;
    std::unique_ptr<PipelineConfigurator> m_pipelineInitiator;

    // Un "pase" de inicialización para almacenar las operaciones
    // Aunque no es un pase de renderizado real, nos sirve para agrupar las operaciones de la misma manera que un RenderPass lo haría.
    std::shared_ptr<RenderPass> m_initPass;

    // Aquí almacenaríamos los recursos creados durante la inicialización
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

	// Esto es un supuesto store del pipeline
	std::map<std::string, PipelineData> m_pipelineStates;
    std::map<std::string, PipelineViewPortData> m_viewports;
    std::map<std::string, PipelineDepthStencilData> m_stencilDesc;

public:
    InitManager();
    ~InitManager() override;

    HRESULT Init(HWND hwnd, int width, int heigth) override;
    HRESULT InitManagers();
    HRESULT InitConfigs();
    HRESULT InitPipelineStates(int width, int height);
    HRESULT InitFinalOperations();
    HRESULT InitMainPipelineOperations(int width, int height);

    const std::string& GetManagerName() const override {
        static const std::string name = "InitManager";
        return name;
    }

    static const std::string& GetStaticManagerName() {
        static const std::string name = "InitManager";
        return name;
    }

    std::map<std::string, PipelineData> GetPipelineStates() { return m_pipelineStates; }

    template<typename T>
    T GetState(std::string stateName) {
        T result;
        auto pair = m_pipelineStates.find(stateName);
        if (pair != m_pipelineStates.end()) {
            result = std::get<T>(pair->second);
        }
        return result;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> GetBackBuffer() {
        auto it = m_pipelineStates.find("Backbuffer");
        if (it != m_pipelineStates.end()) {
            return std::get<Microsoft::WRL::ComPtr<ID3D11Texture2D>>(it->second);
        }
        return nullptr;
    }

    MatrixDefinitionBase::MatrixParams GetConstantsBuffers() const {
        return m_constantsBuffers;
	}

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetRasterizerState(std::string stateName) {
        auto it = m_pipelineStates.find(stateName);
        if (it != m_pipelineStates.end()) {
            return std::get<Microsoft::WRL::ComPtr<ID3D11RasterizerState>>(it->second);
        }
        return nullptr;
	}
    Microsoft::WRL::ComPtr<ID3D11BlendState> GetBlendState(std::string stateName) {
        auto it = m_pipelineStates.find(stateName);
        if (it != m_pipelineStates.end()) {
            return std::get<Microsoft::WRL::ComPtr<ID3D11BlendState>>(it->second);
        }
        return nullptr;
    }
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetStencilState(std::string stateName) {
        auto it = m_pipelineStates.find(stateName);
        if (it != m_pipelineStates.end()) {
            return std::get<Microsoft::WRL::ComPtr<ID3D11DepthStencilState>>(it->second);
        }
        return nullptr;
    }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDepthStencilView(std::string viewName) {
        auto it = m_pipelineStates.find(viewName);
        if (it != m_pipelineStates.end()) {
            PipelineDepthStencilData data = std::get<PipelineDepthStencilData>(it->second);
            return data.stencilViewData;
        }
        return nullptr;
    }
    ID3D11Texture2D* GetTextureViewResource(std::string viewName) {
        auto it = m_pipelineStates.find(viewName);
        if (it != m_pipelineStates.end()) {
            PipelineDepthStencilData data = std::get<PipelineDepthStencilData>(it->second);
            return data.viewTextureData;
        }
        return nullptr;
    }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView(std::string viewName) {
        auto it = m_pipelineStates.find(viewName);
        if (it != m_pipelineStates.end()) {
            PipelineDepthStencilData data = std::get<PipelineDepthStencilData>(it->second);
            return data.shaderViewData;
        }
        return nullptr;
    }
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetDepthStencilState(std::string stateName) {
        auto it = m_pipelineStates.find(stateName);
        if (it != m_pipelineStates.end()) {
            return std::get<Microsoft::WRL::ComPtr<ID3D11DepthStencilState>>(it->second);
        }
        return nullptr;
	}
    Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState(std::string stateName) {
        auto it = m_pipelineStates.find(stateName);
        if (it != m_pipelineStates.end()) {
            return std::get<Microsoft::WRL::ComPtr<ID3D11SamplerState>>(it->second);
        }
        return nullptr;
    }
    SamplerStates GetSamplerStates(std::string shaderName) {
        const std::string pipelineStateName = "samplers";
        PipelineData data = m_pipelineStates[pipelineStateName];
        SamplerStates pipelineSamplers = std::get<SamplerStates>(data);
        return m_shaderManager->GetSamplersStates(StringToWstring(shaderName), pipelineSamplers);
        return std::get<SamplerStates>(data);
    }
    Microsoft::WRL::ComPtr<ID3D11Texture2D> GetTexture2D(std::string textureName) {
        auto it = m_pipelineStates.find(textureName);
        if (it != m_pipelineStates.end()) {
            return std::get<Microsoft::WRL::ComPtr<ID3D11Texture2D>>(it->second);
        }
        return nullptr;
	}
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView(std::string viewName) {
        //auto it = m_pipelineStates.find("RenderTargetView");
        auto it = m_pipelineStates.find(viewName);
        if (it != m_pipelineStates.end()) {
            return std::get<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>(it->second);
        }
        return nullptr;
    }
    /*Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView(std::string viewName) {
        auto it = m_pipelineStates.find(viewName);
        if (it != m_pipelineStates.end()) {
            return std::get<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>(it->second);
        }
        return nullptr;
    }*/
    D3D11_VIEWPORT GetViewport(std::string viewportName) {
        auto it = m_viewports.find(viewportName);
        if (it != m_viewports.end()) {
            return it->second.desc;
        }
        return D3D11_VIEWPORT();
	}

    PipelineDepthStencilData GetDepthStencilData(std::string stencilName) {
        auto it = m_stencilDesc.find(stencilName);
        if (it != m_stencilDesc.end()) {
            return it->second;
        }
        return PipelineDepthStencilData();
    }

    Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain() {
        return m_swapChain;
    }

    std::map<std::string, D3D11_VIEWPORT> GetRenderTargetViewPorts();
    
};