#pragma once

#include "StageBase.h"
#include <d3d11.h>
#include <DeviceManager.h>
#include <memory>
#include <wrl/client.h> // Para ComPtr

namespace RenderPipeline
{
    class RasterizerStage : public StageBase
    {
    public:
        RasterizerStage(std::shared_ptr<DeviceManager> m_deviceManager);

        void SetState(Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerState);
        const void SetViewport(const D3D11_VIEWPORT* viewport) const;
        const void SetViewports(D3D11_VIEWPORT* viewports, UINT viewportCount) const;
    };
}
