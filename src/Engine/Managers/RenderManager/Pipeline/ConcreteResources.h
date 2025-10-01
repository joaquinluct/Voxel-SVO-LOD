#pragma once
#include <d3d11.h>
#include <Defines/Enums/Matrix.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/Structs/PipelineResources.h>
#include <DeviceManager.h>
#include <dxgi.h>
#include <IPipelineViewportState.h>
#include <memory>
#include <string>
#include <Windows.h>
#include <wrl/client.h>

class ConcreteResources
{
public:
    ConcreteResources() = default;
    ~ConcreteResources() = default;

    const void CreateRasterizerState(Microsoft::WRL::ComPtr<ID3D11Device> device, D3D11_RASTERIZER_DESC* desc, Microsoft::WRL::ComPtr<ID3D11RasterizerState>& state);
    const void CreateBackBuffer(Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain, Microsoft::WRL::ComPtr<ID3D11Texture2D>& backBuffer);
    const void CreateViewPort(std::shared_ptr<DeviceManager> deviceManager, std::shared_ptr<IPipelineViewportState> state, D3D11_VIEWPORT& viewport);
    const void CreateRenderTargetView(std::shared_ptr<DeviceManager> deviceManager, Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& renderTargetView);
    const void CreateConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device> device, std::shared_ptr<MatrixDefinition::AnyMatrixBuffer> matrix, const std::string& name, const std::string matrixType, MatrixBufferTypeEnum bufferType, UINT size, UINT slot, PipelineConstantBufferResource& bufferState);

    const void InitDepthSencilView(Microsoft::WRL::ComPtr<ID3D11Device> device, DepthStencilResource* data);
};
