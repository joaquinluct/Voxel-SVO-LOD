#pragma once

#include <d3d11.h>
#include <wrl/client.h> // Para ComPtr

namespace RenderPipeline
{
    class PixelShaderStage
    {
    public:
        PixelShaderStage(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

        void SetShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader);
        void SetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers);
        void SetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews);
        void ResetShaderResources();
        void SetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers);
		void ResetSamplers();

    private:
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    };
}