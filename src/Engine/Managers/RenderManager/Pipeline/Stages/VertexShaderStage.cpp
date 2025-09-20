#include "VertexShaderStage.h"
#include <d3d11.h>
#include <DeviceManager.h>
#include <memory>
#include <Windows.h>
#include <wrl/client.h>

namespace RenderPipeline
{
    VertexShaderStage::VertexShaderStage(std::shared_ptr<DeviceManager> deviceManager)
    {
        this->m_deviceManager = deviceManager;
    }

    VertexShaderStage::~VertexShaderStage()
    {
        // ComPtr se encarga de la liberación de m_context si fuera propietario único.
        // En este diseño, m_context es una copia de un ComPtr externo, por lo que su liberación es gestionada por el propietario.
    }

    // OJO: Para aprender: Hay estos: VSSetShader, HSSetShader, DSSetShader y PSSetShader
    void VertexShaderStage::SetShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader)
    {
        m_deviceManager->GetContext()->VSSetShader(pVertexShader.Get(), nullptr, 0);
    }

    void VertexShaderStage::SetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
    {
        //if (m_context) m_context->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }

    //void VertexShaderStage::UpdateGlobalPassConstants(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Buffer> globalCB, const GlobalPassConstants& data)
    //{
    //    if (!context || !globalCB) return;

    //    D3D11_MAPPED_SUBRESOURCE mappedResource;
    //    HRESULT hr = context->Map(globalCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    //    if (SUCCEEDED(hr))
    //    {
    //        memcpy(mappedResource.pData, &data, sizeof(GlobalPassConstants));
    //        context->Unmap(globalCB.Get(), 0);
    //    }
    //    else
    //    {
    //        // std::cerr << "[VS Stage] Failed to map GlobalPassConstants CB! HRESULT: " << hr << "\n";
    //    }
    //}
}
