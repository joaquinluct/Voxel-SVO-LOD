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

    const void RasterizerStage::SetViewport(const D3D11_VIEWPORT* viewport) const
    {
        if (m_context) m_context->RSSetViewports(1, viewport);
    }
    const void RasterizerStage::SetViewports(D3D11_VIEWPORT* viewport, UINT viewportCount) const
    {
        if (m_context) m_context->RSSetViewports(viewportCount, viewport);
    }

	// CAMBIADO DURANTE LA REFACTORIZACIÓN DEL PIPELINE

    /*void RasterizerStage::SetViewport(D3D11_VIEWPORT& viewport)
    {
        if (m_context) m_context->RSSetViewports(1, &viewport);
    }*/
}