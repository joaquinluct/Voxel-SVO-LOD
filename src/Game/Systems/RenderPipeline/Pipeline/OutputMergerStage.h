#pragma once

#include <d3d11.h>
#include <wrl/client.h> // Para ComPtr

namespace RenderPipeline
{
    class OutputMergerStage
    {
    public:
        OutputMergerStage(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

        void SetRenderTargets(UINT numRTVs, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView);
        void SetBlendState(Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState, const FLOAT BlendFactor[4], UINT SampleMask);
        void SetDepthStencilState(Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState, UINT StencilRef);

        void ClearRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView, const FLOAT ColorRGBA[4]);
        void ClearDepthStencilView(Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil);
        void Present(Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain, UINT SyncInterval, UINT Flags);
    private:
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    };
}