#pragma once

#include <d3d11.h>
#include <wrl/client.h>

struct DirectXContext
{
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

    ID3D11Device* GetDevice() const { return device.Get(); }
    ID3D11DeviceContext* GetContext() const { return context.Get(); }
    IDXGISwapChain* GetSwapChain() const { return swapChain.Get(); }
};
