#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h> // Para ComPtr
#include "../RenderPipeline.h" // Para GlobalPassConstants

namespace RenderPipeline
{
    class VertexShaderStage
    {
    public:
        VertexShaderStage(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
        ~VertexShaderStage(); // Necesario si la etapa gestiona recursos D3D internos no compartidos

        void SetShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader);
        void SetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers);

        void UpdateGlobalPassConstants(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Buffer> globalCB, const GlobalPassConstants& data);

    private:
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    };
}