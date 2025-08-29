#include "PipelineConfigurator.h"
//#include <Defines/ShaderSampler.h>
#include <wrl/client.h>
#include <variant>
#include <vector>
#include <memory>

class SamplerDefinition;

PipelineConfigurator::PipelineConfigurator(
    Microsoft::WRL::ComPtr<ID3D11Device> device,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain)
    : m_device(device), m_context(context), m_swapChain(swapChain)
{
}

HRESULT PipelineConfigurator::ExecuteInitOperation(PipelineOperation& operation) {
    HRESULT hr = S_OK;

    //PipelineData* outData = operation.GetOperationData();

    switch (operation.GetOperationType()) {
    case PipelineOperationType::Device_Init_CreateDeviceAndSwapChain: {
        // Esta operación se ejecutaría una sola vez. No se llama en este configurador
        // ya que este se inicializa *después* de que se crea el dispositivo.
        // Sirve para documentar la operación.
        break;
    }

    case PipelineOperationType::Device_Init_RasterizedState: {
        auto data = operation.GetOperationData<Microsoft::WRL::ComPtr<ID3D11RasterizerState>>();
		auto param = operation.GetOperationParam<PipelineRasteriezeData>();
		ID3D11RasterizerState* state = data.Get();
        hr = m_device->CreateRasterizerState(&param.desc, &state);
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> d = state;
		operation.SetOperationData(d);
        break;
    }

    case PipelineOperationType::Device_Init_BledingState: {
        auto data = operation.GetOperationData<Microsoft::WRL::ComPtr<ID3D11BlendState>>();
        auto param = operation.GetOperationParam<PipelineBledingData>();
		ID3D11BlendState* state = data.Get();
        hr = m_device->CreateBlendState(&param.desc, &state);
        Microsoft::WRL::ComPtr<ID3D11BlendState> d = state;
        operation.SetOperationData(d);
        break;
    }

    case PipelineOperationType::Device_Init_SetSencilView: {
        auto data = operation.GetOperationData<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>>();
        auto param = operation.GetOperationParam<PipelineDepthStencilData>();
        ID3D11Texture2D* depthStencilBuffer = nullptr;
        hr = m_device->CreateTexture2D(&param.desc, nullptr, &depthStencilBuffer);
        if (SUCCEEDED(hr)) {
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
		    ID3D11DepthStencilView* state = nullptr; 
            if (param.hasViewDesc) {
                D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc =  param.viewDesc;
                hr = m_device->CreateDepthStencilView(depthStencilBuffer, &viewDesc, &state);
            }
            else {
                hr = m_device->CreateDepthStencilView(depthStencilBuffer, nullptr, &state);
            }
            if (SUCCEEDED(hr) && param.shaderViewDesc.Format != DXGI_FORMAT_UNKNOWN) {
                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = param.shaderViewDesc;
                hr = m_device->CreateShaderResourceView(depthStencilBuffer, &srvDesc, &shaderResourceView);
            }
            if (FAILED(hr)) {
				return hr; // Si falla, salimos inmediatamente
			}

			param.viewTextureData = depthStencilBuffer;
            param.stencilViewData = state;
			param.shaderViewData = shaderResourceView;

            //Microsoft::WRL::ComPtr<ID3D11DepthStencilView> d = state;
            operation.SetOperationData(param);
        }
		break;
    }

    case PipelineOperationType::Device_Init_SetSencilState: {
        //auto data = operation.GetOperationData<Microsoft::WRL::ComPtr<ID3D11DepthStencilState>>();
		auto param = operation.GetOperationParam<PipelineSetencilStateData>();
        ID3D11DepthStencilState* state = nullptr;
        hr = m_device->CreateDepthStencilState(&param.desc, &state);
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> d = state;
        operation.SetOperationData(d);
		break;
        
    }

    case PipelineOperationType::Device_Init_Samplers: {
        auto param = operation.GetOperationParam<PipelineSamplerSateData>();
        if (param.desc.size() ) {
		    std::map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplerStates;
            for(const ShaderSampler::SamplerDefinition& descPair : param.desc) {
                std::string samplerName = descPair.name;
                Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
                hr = m_device->CreateSamplerState(&descPair.desc, &samplerState);
                if (FAILED(hr)) {
                    return hr; // Si falla, salimos inmediatamente
                }
                samplerStates[samplerName] = samplerState;
			}
            if (samplerStates.size() > 0) {
                operation.SetOperationData(samplerStates);
			}
		}
        break;
    }

    case PipelineOperationType::Device_Init_Viewport: {
        auto param = operation.GetOperationParam<PipelineViewPortData>();
        //operation.SetOperationData(param.desc);
        m_context->RSSetViewports(1, &param.desc);
        break;

    }

    case PipelineOperationType::Device_Init_CreateBackBuffer: {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferTexture;
        // Esta es la forma estándar y segura de obtener una interfaz COM en un ComPtr
        // La macro IID_PPV_ARGS() se encarga de proporcionar el IID y el cast a void**
        hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBufferTexture));

        // Después de que GetBuffer rellene backBufferTexture, lo pasamos a la operación.
        if (SUCCEEDED(hr)) {
            operation.SetOperationData(backBufferTexture);
        }
        break;
    }

    case PipelineOperationType::Device_Init_CreateRenderTargetView: {
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> data = operation.GetOperationData<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>();
        auto param = operation.GetOperationParam<PipelineRenderTargetViewData>();

        ID3D11Texture2D* backBuffer = param.backBuffer.Get();

        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> state = nullptr;
        
        hr = m_device->CreateRenderTargetView(backBuffer, nullptr, &state);

        operation.SetOperationData(state);
        break;
    }
    // Me he quedado aquí.
	// Antes pasaba params y ahora lo he reducido a un solo parmámetro.
    // En este caso no cuadra.

  //  case PipelineOperationType::Mesh_Init_InputLayout: {
  //      auto param = operation.GetOperationParam<PipelineLayoutData>();
		//ID3D11InputLayout* inputLayout = param.data.Get();
  //      hr = m_device->CreateInputLayout(
  //          param.desc, // Array de D3D11_INPUT_ELEMENT_DESC
  //          param.numElements,
  //          param.vertexShader.byteCode->GetBufferPointer(),
  //          param.vertexShader.byteCode->GetBufferSize(), // Obtener el tamaño del bytecode
  //          &inputLayout // El ComPtr del input layout
  //      );
  //      break;
  //  }

    case PipelineOperationType::Device_Init_ConstantsBuffers: {
        break;
    }

    default:
        return S_FALSE;
    }

    operation.SetResult(hr);

    return hr;
}