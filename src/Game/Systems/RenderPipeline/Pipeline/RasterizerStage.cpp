#include "RasterizerStage.h"
// #include <iostream> // Para depuración

namespace RenderPipeline
{
    RasterizerStage::RasterizerStage(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
        : m_context(context)
    {
    }

    void RasterizerStage::SetState(Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerState)
    {
        if (m_context) m_context->RSSetState(pRasterizerState.Get());
    }

    void RasterizerStage::SetViewport(const D3D11_VIEWPORT& viewport)
    {
        if (m_context) m_context->RSSetViewports(1, &viewport);
    }
}