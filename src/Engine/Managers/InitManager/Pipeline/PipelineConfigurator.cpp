#include "PipelineConfigurator.h"
#include <wrl/client.h>
#include <variant>
#include <memory>

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

    case PipelineOperationType::Device_Init_SetencilView: {
        auto data = operation.GetOperationData<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>>();
        auto param = operation.GetOperationParam<PipelineDepthStencilData>();
		ID3D11DepthStencilView* state = data.Get();
        ID3D11Texture2D* depthStencilBuffer = nullptr;
        hr = m_device->CreateTexture2D(&param.desc, nullptr, &depthStencilBuffer);
        if (SUCCEEDED(hr)) {
            hr = m_device->CreateDepthStencilView(depthStencilBuffer, nullptr, &state);
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> d = state;
            operation.SetOperationData(d);
        }
		break;

    }
    case PipelineOperationType::Mesh_Init_SamplerState: {
        auto data = operation.GetOperationData<Microsoft::WRL::ComPtr<ID3D11SamplerState>>();
        auto param = operation.GetOperationParam<PipelineSamplerSateData>();
        ID3D11SamplerState* state = data.Get();
        hr = m_device->CreateSamplerState(&param.desc, &state);
        Microsoft::WRL::ComPtr<ID3D11SamplerState> d = state;
        operation.SetOperationData(d);
        break;
    }

    case PipelineOperationType::Device_Init_Viewport: {
        auto param = operation.GetOperationParam<PipelineViewPortData>();
        m_context->RSSetViewports(1, &param.desc);
        break;

    }

    case PipelineOperationType::Device_Init_CreateBackBuffer: {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> data = operation.GetOperationData<Microsoft::WRL::ComPtr<ID3D11Texture2D>>();
		auto param = operation.GetOperationParam<PipelineBackBufferData>();
        //ID3D11Texture2D** buffer = &data;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> buffer = nullptr;
        hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), buffer.ReleaseAndGetAddressOf());
        /*Microsoft::WRL::ComPtr<ID3D11Texture2D> d = buffer;
        operation.SetOperationData(buffer);*/
        break;
    }

    case PipelineOperationType::Device_Init_CreateRenderTargetView: {
        auto data = operation.GetOperationData<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>();
        auto param = operation.GetOperationParam<PipelineRenderTargetViewData>();
        
        hr = m_device->CreateRenderTargetView(param.backBuffer.Get(), nullptr, &data);

        operation.SetOperationData(data);
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