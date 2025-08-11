#include "InitManager.h"
#include <RenderManager/Pipeline/RenderPipelineExecutor.h>
#include <InitManager/Pipeline/PipelineConfigurator.h>
#include <AssetLocator/AssetLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include <Pipeline/PipelineStateLocator.h>
#include <IPipelineState.h>

#include <Defines/Pass.h>

#include <Pipeline/IPipelineBlendingState.h>
#include <Pipeline/IPipelineRasterizedState.h>
#include <Pipeline/IPipelineStencilState.h>
#include <Pipeline/IPipelineStencilView.h>
#include <Pipeline/IPipelineShaderViewState.h>
#include <Pipeline/IPipelineViewportState.h>

#include <Game/Systems/RenderPipeline/Pipeline/InputAssemblyStage.h>
#include <Game/Systems/RenderPipeline/Pipeline/RasterizerStage.h>
#include <Game/Systems/RenderPipeline/Pipeline/VertexShaderStage.h>
#include <Game/Systems/RenderPipeline/Pipeline/PixelShaderStage.h>
#include <Game/Systems/RenderPipeline/Pipeline/OutputMergerStage.h>

#include "REGISTER_MANAGER_MACRO.h"

REGISTER_MANAGER_TYPE(InitManager, "InitManager")

InitManager::InitManager()
{
}

InitManager::~InitManager()
{
}

HRESULT InitManager::InitManagers() {
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if(!m_deviceManager) {
        return E_FAIL;
	}
	m_shaderManager = ManagerLocator::GetShaderManager();
    if (!m_shaderManager) {
        return E_FAIL;
	}
	return S_OK;
}

HRESULT InitManager::InitConfigs() {
    m_config = ConfigLocator::GetConfig<InitManagerConfig>();
    if (!m_config) {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT InitManager::InitPipelineStates(int width, int height) {
    if (!m_config) {
        return E_FAIL;
    }

	// RASTERIZER STATE
    for (const std::string& stateName : m_config->rasterizedStates) {
        std::shared_ptr<IPipelineRasterizedState> state = PipelineStateLocator::GetPipelineState<IPipelineRasterizedState>(stateName);

		D3D11_RASTERIZER_DESC desc = {};
        
        desc.FillMode = static_cast<D3D11_FILL_MODE>(state->FillMode);
        desc.CullMode = static_cast<D3D11_CULL_MODE>(state->CullMode);
        desc.FrontCounterClockwise = static_cast<BOOL>(state->FrontCounterClockwise);
        desc.AntialiasedLineEnable = static_cast<BOOL>(state->AntialiasedLineEnable);
        desc.DepthClipEnable = static_cast<BOOL>(state->DepthClipEnable);

        PipelineRasteriezeData m_param = {};
		m_param.desc = desc;
		m_param.name = stateName;

        Microsoft::WRL::ComPtr<ID3D11RasterizerState> data = nullptr;
        /*m_pipelineStates[stateName] = data;*/


        m_initPass->AddOperation(PipelineOperationType::Device_Init_RasterizedState, m_param, data);
    }

	// BLENDING STATE
    for (const std::string& stateName : m_config->blendingStates) {
        std::shared_ptr<IPipelineBlendingState> state = PipelineStateLocator::GetPipelineState<IPipelineBlendingState>(stateName);

		D3D11_BLEND_DESC desc = {};

        if (stateName != "DisabledBlending") {
            desc.AlphaToCoverageEnable = static_cast<BOOL>(state->AlphaToCoverageEnable);
            desc.IndependentBlendEnable = static_cast<BOOL>(state->IndependentBlendEnable);
            desc.RenderTarget[0].BlendEnable = static_cast<BOOL>(state->BlendEnable);
            desc.RenderTarget[0].SrcBlend = static_cast<D3D11_BLEND>(state->SrcBlend);
            desc.RenderTarget[0].DestBlend = static_cast<D3D11_BLEND>(state->DestBlend);
            desc.RenderTarget[0].BlendOp = static_cast<D3D11_BLEND_OP>(state->BlendOp);
            desc.RenderTarget[0].SrcBlendAlpha = static_cast<D3D11_BLEND>(state->SrcBlendAlpha);
            desc.RenderTarget[0].DestBlendAlpha = static_cast<D3D11_BLEND>(state->DestBlendAlpha);
            desc.RenderTarget[0].BlendOpAlpha = static_cast<D3D11_BLEND_OP>(state->BlendOpAlpha);
        }
        else {
            bool a = false;
        }
	    desc.RenderTarget[0].RenderTargetWriteMask = static_cast<UINT8>(state->RenderTargetWriteMask);

        PipelineBledingData m_param = {};
		m_param.desc = desc;
        m_param.name = stateName;

        Microsoft::WRL::ComPtr<ID3D11BlendState> data = nullptr;
        //m_pipelineStates[stateName] = data;

        m_initPass->AddOperation(PipelineOperationType::Device_Init_BledingState, m_param, data);
    }

	// STENCIL STATES
    for (const std::string& stateName : m_config->stencilStates) {
        std::shared_ptr<IPipelineStencilState> state = PipelineStateLocator::GetPipelineState<IPipelineStencilState>(stateName);
        PipelineSetencilStateData data = {};
        D3D11_DEPTH_STENCIL_DESC desc = {};
        desc.DepthEnable = static_cast<BOOL>(state->DepthEnable);
        desc.DepthWriteMask = static_cast<D3D11_DEPTH_WRITE_MASK>(state->DepthWriteMask);
        desc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(state->DepthFunc);
        desc.StencilEnable = static_cast<BOOL>(state->StencilEnable);
        desc.StencilReadMask = static_cast<UINT8>(state->StencilReadMask);
        desc.StencilWriteMask = static_cast<UINT8>(state->StencilWriteMask);

		data.desc = desc;
		data.name = stateName;

        m_initPass->AddOperation(PipelineOperationType::Device_Init_SetSencilState, data);
    }

	// STENCIL VIEWS
    for (const std::string& stateName : m_config->stencilViews) {
        std::shared_ptr<IPipelineStencilView> state = PipelineStateLocator::GetPipelineState<IPipelineStencilView>(stateName);

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

        PipelineDepthStencilData m_param = {};

        m_param.hasViewDesc = state->ViewDesc;

        if (state->ViewDesc) {
            m_param.viewDesc = {};
			m_param.viewDesc.Format = static_cast<DXGI_FORMAT>(state->ViewFormat);
			m_param.viewDesc.ViewDimension = static_cast<D3D11_DSV_DIMENSION>(state->ViewDimension);
			m_param.viewDesc.Texture2D.MipSlice = static_cast<UINT>(state->ViewMipSlice);
        }

        if (state->ShaderView.size()) {
            std::shared_ptr<IPipelineShaderViewState> shaderViewState = PipelineStateLocator::GetPipelineState<IPipelineShaderViewState>(state->ShaderView);

            if (shaderViewState != nullptr) {
                m_param.shaderViewDesc = {};
                m_param.shaderViewDesc.Format = static_cast<DXGI_FORMAT>(shaderViewState->Format);
                m_param.shaderViewDesc.ViewDimension = static_cast<D3D11_SRV_DIMENSION>(shaderViewState->ViewDimension);
                m_param.shaderViewDesc.Texture2D.MipLevels = static_cast<UINT>(shaderViewState->MipLevels);
                m_param.shaderViewDesc.Texture2D.MostDetailedMip = static_cast<UINT>(shaderViewState->MostDetailedMip);
            }
        }
		m_param.desc = desc;
        m_param.name = stateName;
        m_param.depth = static_cast<FLOAT>(state->Depth);
        m_param.stencil = static_cast<UINT8>(state->Stencil);


        m_stencilDesc[stateName] = m_param;

        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> data = nullptr;
        //m_pipelineStates[stateName] = data;

		m_initPass->AddOperation(PipelineOperationType::Device_Init_SetSencilView, m_param, data);
    }

	// VIEWPORT STATE
    for (const std::string& stateName : m_config->viewPortStates) {
        std::shared_ptr<IPipelineViewportState> state = PipelineStateLocator::GetPipelineState< IPipelineViewportState>(stateName);

		D3D11_VIEWPORT viewport = {};

		FLOAT width = static_cast<FLOAT>(state->Width);
		FLOAT height = static_cast<FLOAT>(state->Height);

        if (width <= 0 || height <= 0) {
            width = static_cast<FLOAT>(m_deviceManager->GetWidth());
            height = static_cast<FLOAT>(m_deviceManager->GetHeight());
		}

        viewport.TopLeftX = state->TopLeftX;
        viewport.TopLeftY = state->TopLeftY;
        viewport.Width = width;
		viewport.Height = height;
        viewport.MinDepth = state->MinDepth;
        viewport.MaxDepth = state->MaxDepth;

		m_viewports[stateName].desc = viewport;

        PipelineViewPortData m_param = {};
		m_param.desc = viewport;
        m_param.name = stateName;

        m_initPass->AddOperation(PipelineOperationType::Device_Init_Viewport, m_param);
	}

    // SAMPLERS
    std::vector<ShaderSampler::SamplerDefinition> samplersDesc = m_shaderManager->GetAllSamplersDesc();

    if (samplersDesc.size()) {
        PipelineSamplerSateData m_param = {};
        m_param.desc = samplersDesc;
        m_param.data = {};
        m_param.numSamplers = static_cast<UINT>(samplersDesc.size());
        m_param.name = "samplers";
        m_initPass->AddOperation(PipelineOperationType::Device_Init_Samplers, m_param);
    }

    // CONSTANTS BUFFERS
    m_constantsBuffers = {};
    m_constantsBuffers.projectionOrthoMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
        0.0f,                                  // left
        static_cast<float>(width),             // right
        static_cast<float>(height),            // top
        0.0f,                                  // bottom
        0.0f,                                  // nearZ
        1.0f
    );

    return S_OK;
}

HRESULT InitManager::InitMainPipelineOperations(int width, int height)
{
    m_device = m_deviceManager->GetDevice();
    m_context = m_deviceManager->GetContext();
    m_swapChain = m_deviceManager->GetSwapChain();

    if (!m_device || !m_context || !m_swapChain) {
        return E_FAIL;
    }

    PipelineBackBufferData bbParam = {};
    bbParam.name = "Backbuffer";
    Microsoft::WRL::ComPtr<ID3D11Texture2D> data = nullptr;
    m_initPass->AddOperation(PipelineOperationType::Device_Init_CreateBackBuffer, bbParam, data);

    return S_OK;
}

HRESULT InitManager::InitFinalOperations() {
    PipelineRenderTargetViewData rtParam = {};
    rtParam.backBuffer = GetBackBuffer();
    //rtParam.name = "RenderTargetView";
    std::map<std::string, D3D11_VIEWPORT> views = GetRenderTargetViewPorts();
    for (const auto& pView : views) {
		std::string name = pView.first;
        rtParam.name = name;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtData = nullptr;
        m_initPass->AddOperation(PipelineOperationType::Device_Init_CreateRenderTargetView, rtParam, rtData);
	}

    return S_OK;
}

HRESULT InitManager::Init(HWND hwnd, int width, int height)
{
    m_initPass = std::make_shared<RenderPass>(RenderPassType::None, 0, "InitPass");

    HRESULT hr = InitManagers();
    if (FAILED(hr)) {
        return hr;
	}
    hr = InitConfigs();
    if (FAILED(hr)) {
        return hr;
    }

    hr = InitPipelineStates(width, height);
    if (FAILED(hr)) {
        return hr;
    }

	hr = InitMainPipelineOperations(width, height);
    if (FAILED(hr)) {
		return hr;
	}
	
    m_pipelineInitiator = std::make_unique<PipelineConfigurator>(PipelineConfigurator{ m_device, m_context, m_swapChain});

	std::vector<std::shared_ptr<PipelineOperation>> operations = m_initPass->GetOperations();

    for (auto& operation : operations) {
        m_pipelineInitiator->ExecuteInitOperation(*operation);
    }

    operations = m_initPass->GetOperations();

    for (auto& operation : operations) {
        PipelineParameter param = operation->GetOperationParam();
        PipelineData data = operation->GetOperationData();
        
        std::visit([&](auto& p) {
            std::string name = p.name;
            if (!name.empty()) {
                m_pipelineStates[name] = data;
			}
        }, param);
    }

    operations.clear();
    m_initPass = std::make_shared<RenderPass>(RenderPassType::None, 0, "InitPass");

    hr = InitFinalOperations();

    m_pipelineInitiator = std::make_unique<PipelineConfigurator>(PipelineConfigurator{ m_device, m_context, m_swapChain });

    std::vector<std::shared_ptr<PipelineOperation>> operationsFinal = m_initPass->GetOperations();

    for (auto& operation : operationsFinal) {
        m_pipelineInitiator->ExecuteInitOperation(*operation);
    }

    operations = m_initPass->GetOperations();

    for (auto& operation : operations) {
        PipelineParameter param = operation->GetOperationParam();
        PipelineData data = operation->GetOperationData();

        std::visit([&](auto& p) {
            std::string name = p.name;
            m_pipelineStates[name] = data;
            }, param);
    }

    return S_OK;
}

std::map<std::string, D3D11_VIEWPORT> InitManager::GetRenderTargetViewPorts()
{
    std::map<std::string, D3D11_VIEWPORT> result;
    for (auto& pair : m_viewports) {
        // Comprobamos si el PipelineData contiene un ID3D11RenderTargetView
		std::string name = pair.first;
        result[name] = pair.second.desc;
    }
    return result;
}