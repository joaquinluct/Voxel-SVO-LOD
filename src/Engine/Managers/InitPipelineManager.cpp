#include <Windows.h>
#include "InitPipelineManager.h"
#include <AssetLocator/AssetLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Pipeline/PipelineStateLocator.h>

#include <IPipelineState.h>

#include <Defines/Pipeline.h>
#include <Defines/Pass.h>
#include <Defines/FrameStateDefinition.h>

#include <../Includes/FrameStates.h>

#include <Pipeline/IPipelineBlendingState.h>
#include <Pipeline/IPipelineRasterizedState.h>
#include <Pipeline/IPipelineStencilState.h>
#include <Pipeline/IPipelineStencilView.h>
#include <Pipeline/IPipelineShaderViewState.h>
#include <Pipeline/IPipelineViewportState.h>

#include <InitManager/Pipeline/PipelineConfigurator.h>

#include <Services/FrameStateService.h>
#include <DeviceManager.h>
#include <ShaderManager.h>

#include <Config/Base/Managers/InitManagerConfig.h>
#include <Config/Base/Managers/InitPipelineManagerConfig.h>

#include <Config/Base/Pipeline/ShaderMatrixSlotsConfig.h>
#include <Config/Base/Pipeline/ShaderSamplerSlotsConfig.h>
#include <Config/Base/Pipeline/ShaderTextureSlotsConfig.h>

#include <Assets/IAssetShaderConfig.h>

#include <Services/Material.h>

#include <RenderManager/Pipeline/Stages/InputAssemblyStage.h>
#include <RenderManager/Pipeline/Stages/RasterizerStage.h>
#include <RenderManager/Pipeline/Stages/VertexShaderStage.h>
#include <RenderManager/Pipeline/Stages/PixelShaderStage.h>
#include <RenderManager/Pipeline/Stages/OutputMergerStage.h>

#include "REGISTER_MANAGER_MACRO.h"

REGISTER_MANAGER_TYPE(InitPipelineManager, "InitPipelineManager")

InitPipelineManager::InitPipelineManager()
{
}

InitPipelineManager::~InitPipelineManager()
{
}

HRESULT InitPipelineManager::InitPipelineManagers() {
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if(!m_deviceManager) {
        return E_FAIL;
	}
	m_shaderManager = ManagerLocator::GetShaderManager();
    if (!m_shaderManager) {
        return E_FAIL;
	}
    m_FrameStateService = ServiceLocator::GetService<FrameStateService>();
    if (!m_FrameStateService) {
        return E_FAIL;
	} 

	// Obtenemos los estados de Frame
    PipelineFrameState* pipelineState = m_FrameStateService->PipelineState(false);
    CommonFrameState* commonState = m_FrameStateService->CommonState(false);

	// Actualizamos los estados
    pipelineState->SetContext(m_deviceManager->GetContext());
    pipelineState->SetDevice(m_deviceManager->GetDevice());
    pipelineState->SetSwapChain(m_deviceManager->GetSwapChain());
    commonState->SetWidth(m_deviceManager->GetWidth());
	commonState->SetHeight(m_deviceManager->GetHeight());

    return S_OK;
}

HRESULT InitPipelineManager::InitConfigs() {
    m_config = ConfigLocator::GetConfig<InitPipelineManagerConfig>();
    if (!m_config) {
        return E_FAIL;
    }
    m_constantsBuffersSlotsConfig = ConfigLocator::GetConfig<ShaderMatrixSlotsConfig>();
	m_samplersSlotsConfig = ConfigLocator::GetConfig<ShaderSamplerSlotsConfig>();
	m_texturesSlotsConfig = ConfigLocator::GetConfig<ShaderTextureSlotsConfig>();
    return S_OK;
}

HRESULT InitPipelineManager::InitRaserizerStates()
{
    // RASTERIZER STATE
    for (const std::string& stateName : m_config->rasterizedStates) {

        if (StrToLower(stateName) == "none") {
            continue;
		}
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

        m_FrameStateService->PipelineState(false)->CreateRasterizerState(stateName, desc);
    }

    return S_OK;
}

HRESULT InitPipelineManager::InitBlendStates() {
    // BLENDING STATE
    for (const std::string& stateName : m_config->blendingStates) {

        if (StrToLower(stateName) == "none") {
            continue;
        }

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

        desc.RenderTarget[0].RenderTargetWriteMask = static_cast<UINT8>(state->RenderTargetWriteMask);


        Microsoft::WRL::ComPtr<ID3D11BlendState> data = nullptr;

        m_FrameStateService->PipelineState(false)->CreateBlendState(stateName, desc);
    }
    return S_OK;
}

HRESULT InitPipelineManager::InitDepthStencilStates() {
    // STENCIL STATES
    for (const std::string& stateName : m_config->stencilStates) {

        if (StrToLower(stateName) == "none") {
            continue;
        }

        std::shared_ptr<IPipelineStencilState> state = PipelineStateLocator::GetPipelineState<IPipelineStencilState>(stateName);
        D3D11_DEPTH_STENCIL_DESC desc = {};

        desc.DepthEnable = static_cast<BOOL>(state->DepthEnable);
        desc.DepthWriteMask = static_cast<D3D11_DEPTH_WRITE_MASK>(state->DepthWriteMask);
        desc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(state->DepthFunc);
        desc.StencilEnable = static_cast<BOOL>(state->StencilEnable);
        desc.StencilReadMask = static_cast<UINT8>(state->StencilReadMask);
        desc.StencilWriteMask = static_cast<UINT8>(state->StencilWriteMask);

        m_FrameStateService->PipelineState(false)->CreateDepthStencilState(stateName, desc);
    }
    return S_OK;
}
HRESULT InitPipelineManager::InitDepthStencilViews(int width, int height) {
    // STENCIL VIEWS
    for (const std::string& stateName : m_config->stencilViews) {

        if (StrToLower(stateName) == "none") {
            continue;
        }

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

        m_FrameStateService->PipelineState(false)->CreateDepthStencilView(stateName, m_param);		
    }
    return S_OK;
}
HRESULT InitPipelineManager::InitViewports(int width, int height) {
    // VIEWPORT STATE
    for (const std::string& stateName : m_config->viewPortStates) {

        if (StrToLower(stateName) == "none") {
            continue;
        }

        std::shared_ptr<IPipelineViewportState> state = PipelineStateLocator::GetPipelineState<IPipelineViewportState>(stateName);

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

        //m_viewport[stateName].desc = viewport;

        PipelineViewPortData m_param = {};
        m_param.desc = viewport;
        m_param.name = stateName;

        m_FrameStateService->PipelineState(false)->CreateViewport(stateName, viewport);
    }
    return S_OK;
}
HRESULT InitPipelineManager::InitSamplers() {
    // SAMPLERS
    std::vector<ShaderSampler::SamplerDefinition> samplersDesc = m_shaderManager->GetAllSamplersDesc();

    if (samplersDesc.size()) {
        PipelineSamplerSateData m_param = {};
        m_param.desc = samplersDesc;
        m_param.data = {};
        m_param.numSamplers = static_cast<UINT>(samplersDesc.size());
        m_param.name = "samplers";

        for (const ShaderSampler::SamplerDefinition& descPair : m_param.desc) {
            std::string samplerName = descPair.name;
            if (StrToLower(samplerName) == "none") {
                continue;
            }
            m_FrameStateService->PipelineState(false)->CreateSamplerState(samplerName, descPair.desc);
        }
    }
    return S_OK;
}

HRESULT InitPipelineManager::CreateConstantBuffers(std::vector<std::string> slots)
{
	unsigned int slot = 0;
    for (const std::string& constantBufferName : slots) {
        if (!constantBufferName.size() || StrToLower(constantBufferName) == "none") {
            continue;
        }

        std::shared_ptr<MatrixDefinition::AnyMatrixBuffer> const constantsBuffer = m_shaderManager->GetConstantsBuffer(constantBufferName);

        if (!constantsBuffer) {
            continue;
        }

        unsigned int vertexSize = 0;
        std::string matrixType = "";
        std::visit([&](auto& currentVertex) {
            vertexSize = currentVertex.Size();
			matrixType = currentVertex.MatrixType();
            }, *constantsBuffer);

        m_FrameStateService->PipelineState(false)->CreateConstantBuffer(constantBufferName, matrixType, vertexSize, slot);
		slot++;
    }
    return S_OK;
}

HRESULT InitPipelineManager::SetConstantBuffers(std::vector<std::string> slots) {
    m_FrameStateService->PipelineState(false)->SetConstantsBuffers(slots);
    return S_OK;
}
HRESULT InitPipelineManager::CreateTextures(std::vector<std::string> slots) {
	//unsigned int slot = 0;
 //   for (const std::string& textureName : slots) {
 //       if (textureName.size()) {
	//		Material* material = new Material();
	//		material->SetShaderName()
	//		m_FrameStateService->PipelineState(false)->SetTexture(slotName, slot);
 //           //m_FrameStateService->PipelineState(false)->SetTexture(slot);
	//		slot++;
 //       }
	//}

    return S_OK;
}
HRESULT InitPipelineManager::CreateSamplers(std::vector<std::string> slots) {
    unsigned int slot = 0;
    for(const std::string& slotName : slots) {
        if (slotName.size()) {
            m_FrameStateService->PipelineState(false)->SetSamplerState(slotName, slot);
			slot++;
        }
	}
    return S_OK;
}
HRESULT InitPipelineManager::InitShaders() {
    std::vector<std::string> constanstBuffersSlots = m_constantsBuffersSlotsConfig->slots;
	std::vector<std::string> samplersSlots = m_samplersSlotsConfig->slots;
	std::vector<std::string> texturesSlots = m_texturesSlotsConfig->slots;
	CreateSamplers(samplersSlots);
    CreateConstantBuffers(constanstBuffersSlots);
	CreateTextures(texturesSlots);
	SetConstantBuffers(constanstBuffersSlots);

    //std::shared_ptr<IAssetShaderConfig> shaderConfig = ConfigLocator::GetConfig<IAssetShaderConfig>();

    /*if (!shaderConfig) {
        return E_FAIL;
    }*/
    return S_OK;
}

HRESULT InitPipelineManager::InitPipelineStates(int width, int height) {
    if (!m_config) {
        return E_FAIL;
    }
	HRESULT hr = InitRaserizerStates();
    if (FAILED(hr)) {
        return hr;
	}
    hr = InitBlendStates();
    if (FAILED(hr)) {
        return hr;
    }
    hr = InitDepthStencilStates();
    if (FAILED(hr)) {
        return hr;
    }
    hr = InitDepthStencilViews(width, height);
    if (FAILED(hr)) {
        return hr;
    }
    hr = InitViewports(width, height);
    if (FAILED(hr)) {
        return hr;
    }  
	hr = InitSamplers();
    if (FAILED(hr)) {
        return hr;
    }
    hr = InitShaders();
    if (FAILED(hr)) {
        return hr;
	}

    m_FrameStateService->Update(0.0f);
    m_FrameStateService->SwapBuffersContent();
    m_FrameStateService->SwapBuffers();
	// Finalmente hacemos un SwapBuffers (PERO DEL CONTENIDO) para dejar todo listo

    //m_FrameStateService->SwapBuffers();

	//// Matrices (De momento solo la Ortogr fica)
 //   std::shared_ptr<MatrixParams> bData = std::make_shared<MatrixParams>();
 //   bData->projectionOrthoMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
 //       0.0f,                                  // left
 //       static_cast<float>(width),             // right
 //       static_cast<float>(height),            // top
 //       0.0f,                                  // bottom
 //       0.0f,                                  // nearZ
 //       1.0f
 //   );

    return S_OK;
}

HRESULT InitPipelineManager::InitMainPipelineOperations(int width, int height)
{
    m_FrameStateService->PipelineState(false)->CreateBackBuffer();

    /*PipelineBackBufferData bbParam = {};
    bbParam.name = "Backbuffer";
    Microsoft::WRL::ComPtr<ID3InitMainPipelineOperationsD11Texture2D> data = nullptr;
    m_initPass->AddOperation(PipelineOperationType::Device_Init_CreateBackBuffer, bbParam, data);*/

    return S_OK;
}

HRESULT InitPipelineManager::InitFinalOperations() {

    m_FrameStateService->PipelineState(false)->CreateRenderTargetsView();

    //PipelineRenderTargetViewData rtParam = {};
    //rtParam.backBuffer = GetBackBuffer();
    //rtParam.name = "RenderTargetView";
    /*std::map<std::string, D3D11_VIEWPORT> views = GetRenderTargetViewPorts();
    for (const auto& pView : views) {
		std::string name = pView.first;
        rtParam.name = name;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtData = nullptr;
		m_FrameStateService->CommonState()->CreateRenderTargetView(name, rtParam.backBuffer.Get());
        m_initPass->AddOperation(PipelineOperationType::Device_Init_CreateRenderTargetView, rtParam, rtData);
	}*/

    return S_OK;
}

HRESULT InitPipelineManager::Init(HWND* hwnd, int width, int height)
{
    //m_initPass = std::make_shared<RenderPass>(RenderPassType::None, 0, "InitPass");

    HRESULT hr = InitPipelineManagers();
    if (FAILED(hr)) {
        return hr;
	}
    hr = InitConfigs();
    if (FAILED(hr)) {
        return hr;
    }

	hr = InitMainPipelineOperations(width, height);
    if (FAILED(hr)) {
		return hr;
	}

    hr = InitPipelineStates(width, height);
    if (FAILED(hr)) {
        return hr;
    }

    InitFinalOperations();

    return S_OK;
}

//const std::map<std::string, D3D11_VIEWPORT> InitPipelineManager::GetRenderTargetViewPorts() const 
//{
//    std::map<std::string, D3D11_VIEWPORT> result;
//    for (auto& pair : m_viewports) {
//        // Comprobamos si el PipelineData contiene un ID3D11RenderTargetView
//		std::string name = pair.first;
//        result[name] = pair.second.desc;
//    }
//    return result;
//}