#include "InputAssemblyStage.h"
// #include <iostream> // Para depuración

namespace RenderPipeline
{
    InputAssemblyStage::InputAssemblyStage(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
        : m_context(context)
    {
    }

    void InputAssemblyStage::SetInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout)
    {
        if (m_context) m_context->IASetInputLayout(pInputLayout.Get());
    }

    void InputAssemblyStage::SetVertexBuffers(UINT StartSlot, UINT NumBuffers, Microsoft::WRL::ComPtr<ID3D11Buffer> ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets)
    {
        // ppVertexBuffers ya es un array de punteros crudos, por lo que Get() no es necesario aquí.
        // Si ppVertexBuffers fuera un array de ComPtr, necesitarías transformar el array.
		ID3D11Buffer* ppVertexBuffersArray = ppVertexBuffers.Get();
        if (m_context) m_context->IASetVertexBuffers(StartSlot, NumBuffers, &ppVertexBuffersArray, pStrides, pOffsets);
    }

    void InputAssemblyStage::ResetVertexBuffers()
    {   
        if (m_context) {
            const UINT pStrides = 0;
            const UINT pOffsets = 0;        
            m_context->IASetVertexBuffers(0, 1, nullptr, &pStrides, &pOffsets);
        }
    }

    void InputAssemblyStage::SetIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
    {
        if (m_context) m_context->IASetIndexBuffer(pIndexBuffer.Get(), Format, Offset);
    }

    void InputAssemblyStage::ResetIndexBuffers()
    {
        if (m_context) m_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    }

    void InputAssemblyStage::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology)
    {
        if (m_context) m_context->IASetPrimitiveTopology(Topology);
    }
}