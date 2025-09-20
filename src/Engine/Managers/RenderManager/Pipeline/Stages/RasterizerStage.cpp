#include "RasterizerStage.h"
#include <d3d11.h>
#include <DeviceManager.h>
#include <memory>
#include <Windows.h>
#include <wrl/client.h>
// #include <iostream> // Para depuración

namespace RenderPipeline
{
    RasterizerStage::RasterizerStage(std::shared_ptr<DeviceManager> m_deviceManager) {
        this->m_deviceManager = m_deviceManager;
    }

    void RasterizerStage::SetState(Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerState)
    {
        m_deviceManager->GetContext()->RSSetState(pRasterizerState.Get());
    }

    const void RasterizerStage::SetViewport(const D3D11_VIEWPORT* viewport) const
    {
        m_deviceManager->GetContext()->RSSetViewports(1, viewport);
    }
    const void RasterizerStage::SetViewports(D3D11_VIEWPORT* viewport, UINT viewportCount) const
    {
        m_deviceManager->GetContext()->RSSetViewports(viewportCount, viewport);
    }

    // CAMBIADO DURANTE LA REFACTORIZACIÓN DEL PIPELINE

    /*void RasterizerStage::SetViewport(D3D11_VIEWPORT& viewport)
    {
        if (m_context) m_context->RSSetViewports(1, &viewport);
    }*/
}
