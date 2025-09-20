#include "InputAssemblyStage.h"
// #include <iostream> // Para depuración

namespace RenderPipeline
{
    InputAssemblyStage::InputAssemblyStage(std::shared_ptr<DeviceManager> deviceManager)
    {
        this->m_deviceManager = deviceManager;
    }

    void InputAssemblyStage::SetInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout)
    {
        m_deviceManager->GetContext()->IASetInputLayout(pInputLayout.Get());
    }

    void InputAssemblyStage::SetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets)
    {
        // ppVertexBuffers ya es un array de punteros crudos, por lo que Get() no es necesario aquí.
        // Si ppVertexBuffers fuera un array de ComPtr, necesitarías transformar el array.
        //ID3D11Buffer* vBuffer = ppVertexBuffers.Get();        
        m_deviceManager->GetContext()->IASetVertexBuffers(StartSlot, NumBuffers, &ppVertexBuffers, pStrides, pOffsets);
    }

    void InputAssemblyStage::ResetVertexBuffers()
    {

        const UINT pStrides = 0;
        const UINT pOffsets = 0;
        m_deviceManager->GetContext()->IASetVertexBuffers(0, 1, nullptr, &pStrides, &pOffsets);

    }

    void InputAssemblyStage::SetIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
    {
        m_deviceManager->GetContext()->IASetIndexBuffer(pIndexBuffer.Get(), Format, Offset);
    }

    void InputAssemblyStage::ResetIndexBuffers()
    {
        m_deviceManager->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    }

    void InputAssemblyStage::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology)
    {
        m_deviceManager->GetContext()->IASetPrimitiveTopology(Topology);
    }
}
