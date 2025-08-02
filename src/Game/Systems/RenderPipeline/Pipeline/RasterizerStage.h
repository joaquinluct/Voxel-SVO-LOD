#pragma once

#include <d3d11.h>
#include <wrl/client.h> // Para ComPtr

namespace RenderPipeline
{
    class RasterizerStage
    {
    public:
        RasterizerStage(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

        void SetState(Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerState);
        void SetViewport(D3D11_VIEWPORT& viewport);

    private:
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    };
}