#pragma once

#include <d3d11.h>
#include <wrl/client.h> // Para ComPtr

namespace RenderPipeline
{
    class InputAssemblyStage
    {
    public:
        InputAssemblyStage(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

        void SetInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout);
        void SetVertexBuffers(UINT StartSlot, UINT NumBuffers, Microsoft::WRL::ComPtr<ID3D11Buffer> ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets);
        void SetIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer, DXGI_FORMAT Format, UINT Offset);
        void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology);

    private:
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    };
}