#pragma once

#include "FrameStateBase.h"
#include <DirectXMath.h>
#include <wrl/client.h>

class DeviceFrameState : public FrameStateBase {
public:
    const float* GetClearColor() const { return clearColor; }
    void SetClearColor(const DirectX::XMFLOAT4& color) {
        clearColor[0] = color.x;
        clearColor[1] = color.y;
        clearColor[2] = color.z;
        clearColor[3] = color.w;
    }

    void SetRenderTargetView(ID3D11RenderTargetView* rtv) { RenderTargetView = rtv; }
    ID3D11RenderTargetView* GetRenderTargetView() const { return RenderTargetView.Get(); }

    void SetDepthStencilView(ID3D11DepthStencilView* dsv) { DepthStencilView = dsv; }
    ID3D11DepthStencilView* GetDepthStencilView() const { return DepthStencilView.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
    float clearColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
};

