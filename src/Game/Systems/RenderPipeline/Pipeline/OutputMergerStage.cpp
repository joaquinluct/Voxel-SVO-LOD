#include "OutputMergerStage.h"
// #include <iostream> // Para depuración

namespace RenderPipeline
{
    OutputMergerStage::OutputMergerStage(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
        : m_context(context)
    {
    }

    void OutputMergerStage::SetRenderTargets(UINT numRTVs, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView)
    {
        if (m_context) m_context->OMSetRenderTargets(numRTVs, ppRenderTargetViews, pDepthStencilView);
    }

    void OutputMergerStage::SetBlendState(Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState, const FLOAT BlendFactor[4], UINT SampleMask)
    {
        if (m_context) m_context->OMSetBlendState(pBlendState.Get(), nullptr, 0xFFFFFFFF);
    }

    void OutputMergerStage::SetDepthStencilState(Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState, UINT StencilRef)
    {
        if (m_context) m_context->OMSetDepthStencilState(pDepthStencilState.Get(), StencilRef);
    }

    void OutputMergerStage::ClearRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView, const FLOAT ColorRGBA[4])
    {
        if (m_context) m_context->ClearRenderTargetView(pRenderTargetView.Get(), ColorRGBA);
    }

    void OutputMergerStage::ClearDepthStencilView(Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil)
    {
        if (m_context) m_context->ClearDepthStencilView(pDepthStencilView.Get(), ClearFlags, Depth, Stencil);
    }
    void OutputMergerStage::Present(Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain, UINT syncInterval, UINT flags)
    {
        if (swapChain) swapChain.Get()->Present(syncInterval, flags);
    }
}