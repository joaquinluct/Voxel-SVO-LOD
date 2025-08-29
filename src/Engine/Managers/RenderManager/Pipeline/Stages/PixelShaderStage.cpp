#include "PixelShaderStage.h"
// #include <iostream> // Para depuración

namespace RenderPipeline
{
    PixelShaderStage::PixelShaderStage(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
        : m_context(context)
    {
    }

    // OJO: Para aprender: Hay estos: VSSetShader, HSSetShader, DSSetShader y PSSetShader
    void PixelShaderStage::SetShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader)
    {
        if (m_context) m_context->PSSetShader(pPixelShader.Get(), nullptr, 0);
    }

    void PixelShaderStage::SetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
    {
        if (m_context) m_context->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }

    void PixelShaderStage::SetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
    {
        if (m_context) m_context->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }

    void PixelShaderStage::ResetShaderResources()
    {
        if (m_context) {
            ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
            m_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRVs);
        }
    }
    

    void PixelShaderStage::SetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers)
    {
        if (m_context) m_context->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    }

    void PixelShaderStage::ResetSamplers()
    {
        if (m_context) {
            ID3D11SamplerState* nullSamplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
            m_context->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, nullSamplers);
        }
    }
}