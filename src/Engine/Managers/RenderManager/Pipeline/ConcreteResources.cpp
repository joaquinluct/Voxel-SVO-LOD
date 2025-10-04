#include "ConcreteResources.h"
#include <combaseapi.h>
#include <d3d11.h>
#include <Defines/Enums/Matrix.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/Structs/Pipeline/PipelineResources.h>
#include <dxgi.h>
#include <IPipelineViewportState.h>
#include <Managers/DeviceManager.h>
#include <memory>
#include <string>
#include <Util/MathUtil.h>
#include <utility>
#include <Windows.h>
#include <wrl/client.h>

const void ConcreteResources::CreateRasterizerState(Microsoft::WRL::ComPtr<ID3D11Device> device, D3D11_RASTERIZER_DESC* desc, Microsoft::WRL::ComPtr<ID3D11RasterizerState>& state) {
    if (device) {
        ID3D11RasterizerState* newState = nullptr;
        D3D11_RASTERIZER_DESC rasterizerDesc = {};
        rasterizerDesc.AntialiasedLineEnable = desc->AntialiasedLineEnable;
        rasterizerDesc.CullMode = static_cast<D3D11_CULL_MODE>(desc->CullMode);
        rasterizerDesc.DepthBias = desc->DepthBias;
        rasterizerDesc.DepthBiasClamp = desc->DepthBiasClamp;
        rasterizerDesc.DepthClipEnable = desc->DepthClipEnable;
        rasterizerDesc.FillMode = static_cast<D3D11_FILL_MODE>(desc->FillMode);
        rasterizerDesc.FrontCounterClockwise = desc->FrontCounterClockwise;
        rasterizerDesc.MultisampleEnable = desc->MultisampleEnable;
        rasterizerDesc.ScissorEnable = desc->ScissorEnable;
        rasterizerDesc.SlopeScaledDepthBias = desc->SlopeScaledDepthBias;
        HRESULT hr = device->CreateRasterizerState(&rasterizerDesc, &newState);
        state = Microsoft::WRL::ComPtr<ID3D11RasterizerState>(newState);
        if (FAILED(hr)) {
            // Handle error
        }
    }
}

const void ConcreteResources::CreateBackBuffer(Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain, Microsoft::WRL::ComPtr<ID3D11Texture2D>& backBuffer) {
    if (swapChain) {
        //HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_backBuffer.GetAddressOf()));
        HRESULT hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        if (FAILED(hr)) {
            // Handle error
        }
    }
}

const void ConcreteResources::CreateViewPort(std::shared_ptr<DeviceManager> deviceManager, std::shared_ptr<IPipelineViewportState> state, D3D11_VIEWPORT& viewport) {

    FLOAT width = static_cast<FLOAT>(state->Width);
    FLOAT height = static_cast<FLOAT>(state->Height);

    if (width <= 0 || height <= 0) {
        width = static_cast<FLOAT>(deviceManager->GetWidth());
        height = static_cast<FLOAT>(deviceManager->GetHeight());
    }

    viewport.TopLeftX = state->TopLeftX;
    viewport.TopLeftY = state->TopLeftY;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = state->MinDepth;
    viewport.MaxDepth = state->MaxDepth;

    //deviceManager->GetContext()->RSSetViewports(1, &viewport);
}

const void ConcreteResources::CreateRenderTargetView(std::shared_ptr<DeviceManager> deviceManager, Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& renderTargetView) {
    if (backBuffer) {
        HRESULT hr = deviceManager->GetDevice()->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
        if (FAILED(hr)) {
            // Handle error
        }
    }
}

const void ConcreteResources::InitDepthSencilView(Microsoft::WRL::ComPtr<ID3D11Device> device, DepthStencilResource* data) {
    if (device) {
        ID3D11Texture2D* depthStencilBuffer = nullptr;
        HRESULT hr = device->CreateTexture2D(&data->desc, nullptr, &depthStencilBuffer);
        if (SUCCEEDED(hr)) {
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> stencilView;

            if (data->hasViewDesc) {
                D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc = data->viewDesc;
                hr = device->CreateDepthStencilView(depthStencilBuffer, &viewDesc, &stencilView);
            }
            else {
                hr = device->CreateDepthStencilView(depthStencilBuffer, nullptr, &stencilView);
            }

            if (SUCCEEDED(hr) && data->shaderViewDesc.Format != DXGI_FORMAT_UNKNOWN) {
                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = data->shaderViewDesc;
                hr = device->CreateShaderResourceView(depthStencilBuffer, &srvDesc, &shaderResourceView);
            }
            if (FAILED(hr)) {
                return;
            }

            data->viewTextureData = depthStencilBuffer;
            data->stencilViewData = std::move(stencilView);
            data->shaderViewData = std::move(shaderResourceView);
            data->desc = data->desc;
            data->viewDesc = data->viewDesc;
            data->shaderViewDesc = data->shaderViewDesc;
            data->hasViewDesc = data->hasViewDesc;
        }
    }
}

const void ConcreteResources::CreateConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device> device, std::shared_ptr<MatrixDefinition::AnyMatrixBuffer> matrix, const std::string& name, const std::string matrixType, MatrixBufferTypeEnum bufferType, UINT size, UINT slot, PipelineConstantBufferResource& bufferState) {
    if (device) {

        D3D11_BUFFER_DESC bufferDesc = {};

        size = GetNext16Multiple(size);

        if (bufferType == MatrixBufferTypeEnum::Static) {
            bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            bufferDesc.ByteWidth = size;
            bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bufferDesc.CPUAccessFlags = 0;
            bufferDesc.MiscFlags = 0;
            bufferDesc.StructureByteStride = 0;
        }
        else {
            bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            bufferDesc.ByteWidth = size;
            bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bufferDesc.MiscFlags = 0;
            bufferDesc.StructureByteStride = 0;
        }

        HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, bufferState.buffer.ReleaseAndGetAddressOf());

        if (SUCCEEDED(hr)) {
            bufferState.name = name;
            bufferState.matrixType = matrixType;
            bufferState.slot = slot;
            //bufferState.buffer = constantBuffer;
            //constantBuffer.Attach(bufferState.buffer.Get());
            bufferState.bufferType = bufferType;
            bufferState.data = *matrix;
        }
    }
}
//const void ConcreteResources::BindConstantBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, std::shared_ptr<MatrixDefinition::AnyMatrixBuffer> matrix, const std::string& name, const std::string matrixType, MatrixBufferTypeEnum bufferType, UINT size, UINT slot, PipelineConstantBufferResource& bufferState) {
//    context->VSSetConstantBuffers(slot, 1, pBuffers.data());
//    context->PSSetConstantBuffers(slot, 1, pBuffers.data());
//}
