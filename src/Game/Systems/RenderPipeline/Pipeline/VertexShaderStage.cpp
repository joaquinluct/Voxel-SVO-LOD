#include "VertexShaderStage.h"
// #include <iostream> // Para depuración

namespace RenderPipeline
{
    VertexShaderStage::VertexShaderStage(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
        : m_context(context)
    {
    }

    VertexShaderStage::~VertexShaderStage()
    {
        // ComPtr se encarga de la liberación de m_context si fuera propietario único.
        // En este diseño, m_context es una copia de un ComPtr externo, por lo que su liberación es gestionada por el propietario.
    }

    void VertexShaderStage::SetShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader)
    {
        if (m_context) m_context->VSSetShader(pVertexShader.Get(), nullptr, 0);
    }

    void VertexShaderStage::SetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
    {
        if (m_context) m_context->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }

    void VertexShaderStage::UpdateGlobalPassConstants(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Buffer> globalCB, const GlobalPassConstants& data)
    {
        if (!context || !globalCB) return;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT hr = context->Map(globalCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (SUCCEEDED(hr))
        {
            memcpy(mappedResource.pData, &data, sizeof(GlobalPassConstants));
            context->Unmap(globalCB.Get(), 0);
        }
        else
        {
            // std::cerr << "[VS Stage] Failed to map GlobalPassConstants CB! HRESULT: " << hr << "\n";
        }
    }
}