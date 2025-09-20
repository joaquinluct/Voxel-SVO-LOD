#include "OutputMergerStage.h"
#include <d3d11.h>
#include <DeviceManager.h>
#include <memory>
#include <Windows.h>
#include <wrl/client.h>

namespace RenderPipeline
{
    OutputMergerStage::OutputMergerStage(std::shared_ptr<DeviceManager> deviceManager)
    {
        m_deviceManager = deviceManager;
    }

    void OutputMergerStage::SetRenderTargets(UINT numRTVs, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView)
    {
        m_deviceManager->GetContext()->OMSetRenderTargets(numRTVs, ppRenderTargetViews, pDepthStencilView);
    }

    //void OutputMergerStage::SetBlendState(Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState, const FLOAT BlendFactor[4], UINT SampleMask)
    void OutputMergerStage::SetBlendState(Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState)
    {
        m_deviceManager->GetContext()->OMSetBlendState(pBlendState.Get(), nullptr, 0xFFFFFFFF);
    }

    void OutputMergerStage::SetDepthStencilState(Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState, UINT StencilRef)
    {
        m_deviceManager->GetContext()->OMSetDepthStencilState(pDepthStencilState.Get(), StencilRef);
    }

    void OutputMergerStage::ClearRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView, const FLOAT ColorRGBA[4])
    {
        ID3D11RenderTargetView* pRTV = pRenderTargetView.Get();
        m_deviceManager->GetContext()->ClearRenderTargetView(pRTV, ColorRGBA);
    }

    void OutputMergerStage::ClearDepthStencilView(Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil)
    {
        m_deviceManager->GetContext()->ClearDepthStencilView(pDepthStencilView.Get(), ClearFlags, Depth, Stencil);
    }
    void OutputMergerStage::Present(Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain, UINT syncInterval, UINT flags)
    {
        if (swapChain) swapChain.Get()->Present(syncInterval, flags);
    }
}
