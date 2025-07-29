#include "InitManager.h"
#include "REGISTER_MANAGER_MACRO.h"
#include <RenderManager/Pipeline/RenderPipelineExecutor.h>
#include <InitManager/Pipeline/PipelineConfigurator.h>
#include <AssetLocator/AssetLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include <Pipeline/PipelineStateLocator.h>
#include <IPipelineState.h>

#include <Pipeline/IPipelineBlendingState.h>
#include <Pipeline/IPipelineRasterizedState.h>
#include <Pipeline/IPipelineStencilState.h>
#include <Pipeline/IPipelineViewportState.h>

#include <Game/Systems/RenderPipeline/Pipeline/InputAssemblyStage.h>
#include <Game/Systems/RenderPipeline/Pipeline/RasterizerStage.h>
#include <Game/Systems/RenderPipeline/Pipeline/VertexShaderStage.h>
#include <Game/Systems/RenderPipeline/Pipeline/PixelShaderStage.h>
#include <Game/Systems/RenderPipeline/Pipeline/OutputMergerStage.h>

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
        std::shared_ptr<IPipelineRasterizedState> state = PipelineStateLocator::GetPipelineState< IPipelineRasterizedState>(stateName);

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
        std::shared_ptr<IPipelineBlendingState> state = PipelineStateLocator::GetPipelineState< IPipelineBlendingState>(stateName);

		D3D11_BLEND_DESC desc = {};

		desc.AlphaToCoverageEnable = static_cast<BOOL>(state->AlphaToCoverageEnable);
		desc.IndependentBlendEnable = static_cast<BOOL>(state->IndependentBlendEnable);
        desc.RenderTarget[0].BlendEnable = static_cast<BOOL>(state->BlendEnable);
        desc.RenderTarget[0].SrcBlend = static_cast<D3D11_BLEND>(state->SrcBlend);
        desc.RenderTarget[0].DestBlend = static_cast<D3D11_BLEND>(state->DestBlend);
        desc.RenderTarget[0].BlendOp = static_cast<D3D11_BLEND_OP>(state->BlendOp);
        desc.RenderTarget[0].SrcBlendAlpha = static_cast<D3D11_BLEND>(state->SrcBlendAlpha);
        desc.RenderTarget[0].DestBlendAlpha = static_cast<D3D11_BLEND>(state->DestBlendAlpha);
        desc.RenderTarget[0].BlendOpAlpha = static_cast<D3D11_BLEND_OP>(state->BlendOpAlpha);

        PipelineBledingData m_param = {};
		m_param.desc = desc;
        m_param.name = stateName;

        Microsoft::WRL::ComPtr<ID3D11BlendState> data = nullptr;
        //m_pipelineStates[stateName] = data;

        m_initPass->AddOperation(PipelineOperationType::Device_Init_BledingState, m_param, data);
    }

	// STENCIL STATE
    for (const std::string& stateName : m_config->stencilStates) {
        std::shared_ptr<IPipelineStencilState> state = PipelineStateLocator::GetPipelineState< IPipelineStencilState>(stateName);

		D3D11_TEXTURE2D_DESC desc = {};

        desc.ArraySize = static_cast<UINT>(state->ArraySize);
        desc.BindFlags = static_cast<UINT>(state->BindFlags);
        desc.Format = static_cast<DXGI_FORMAT>(state->Format);
        desc.MipLevels = static_cast<UINT>(state->MipLevels);
        desc.SampleDesc.Count = static_cast<UINT>(state->SampleCount);
        desc.SampleDesc.Quality = static_cast<UINT>(state->SampleQuality);
        desc.Usage = static_cast<D3D11_USAGE>(state->Usage);
        desc.Width = width;
        desc.Height= height;

        PipelineDepthStencilData m_param = {};
		m_param.desc = desc;
        m_param.name = stateName;

        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> data = nullptr;
        //m_pipelineStates[stateName] = data;

		m_initPass->AddOperation(PipelineOperationType::Device_Init_SetencilView, m_param, data);
    }

    for (const std::string& stateName : m_config->viewPortStates) {
        std::shared_ptr<IPipelineViewportState> state = PipelineStateLocator::GetPipelineState< IPipelineViewportState>(stateName);

		D3D11_VIEWPORT viewport = {};

        viewport.TopLeftX = state->TopLeftX;
        viewport.TopLeftY = state->TopLeftY;
        viewport.Width = state->Width;
        viewport.Height = state->Height;
        viewport.MinDepth = state->MinDepth;
        viewport.MaxDepth = state->MaxDepth;

        PipelineViewPortData m_param = {};
		m_param.desc = viewport;
        m_param.name = stateName;

        m_initPass->AddOperation(PipelineOperationType::Device_Init_Viewport, m_param);
	}

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
    /*rtParam.backBuffer = data;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtData = nullptr;
      m_initPass->AddOperation(PipelineOperationType::Device_Init_CreateRenderTargetView, rtParam, rtData);*/

    return S_OK;
}

HRESULT InitManager::Init(HWND hwnd, int width, int height)
{
    m_params = {};
    m_initPass = std::make_shared<RenderPass>(RenderPassType::Unknown, 0, "InitPass");

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
            m_pipelineStates[name] = data;
        }, param);

        //m_pipelineStates[std::get<std::string>param.name] = data;
        m_pipelineInitiator->ExecuteInitOperation(*operation);
    }

    operations.clear();

    hr = InitFinalOperations();

    m_pipelineInitiator = std::make_unique<PipelineConfigurator>(PipelineConfigurator{ m_device, m_context, m_swapChain });

    std::vector<std::shared_ptr<PipelineOperation>> operationsFinal = m_initPass->GetOperations();

    for (auto& operation : operationsFinal) {
        m_pipelineInitiator->ExecuteInitOperation(*operation);
    }

    return S_OK;
}