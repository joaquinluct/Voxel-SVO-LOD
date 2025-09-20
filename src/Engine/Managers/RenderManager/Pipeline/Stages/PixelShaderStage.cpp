#include "PixelShaderStage.h"
#include <d3d11.h>
#include <DeviceManager.h>
#include <memory>
#include <Windows.h>
#include <wrl/client.h>

namespace RenderPipeline
{
    PixelShaderStage::PixelShaderStage(std::shared_ptr<DeviceManager> deviceManager)
    {
        this->m_deviceManager = deviceManager;
    }

    // OJO: Para aprender: Hay estos: VSSetShader, HSSetShader, DSSetShader y PSSetShader
    void PixelShaderStage::SetShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader)
    {
        m_deviceManager->GetContext()->PSSetShader(pPixelShader.Get(), nullptr, 0);
    }

    void PixelShaderStage::SetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
    {
        m_deviceManager->GetContext()->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }

    void PixelShaderStage::SetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
    {
        m_deviceManager->GetContext()->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
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
        m_deviceManager->GetContext()->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    }

    void PixelShaderStage::ResetSamplers()
    {
        if (m_context) {
            ID3D11SamplerState* nullSamplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
            m_context->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, nullSamplers);
        }
    }
}
