#include "RingBuffer.h"
#include <cstring>
#include <d3d11.h>
#include <Windows.h>

// Implementación de GetBindFlags (simplificada)
D3D11_BIND_FLAG RingBuffer::GetBindFlags(BufferType type) const {
    switch (type) {
    case BufferType::VERTEX: return D3D11_BIND_VERTEX_BUFFER;
    case BufferType::INDEX: return D3D11_BIND_INDEX_BUFFER;
    case BufferType::CONSTANT: return D3D11_BIND_CONSTANT_BUFFER;
    }
    return static_cast<D3D11_BIND_FLAG>(0);
}

// Implementación de Init
HRESULT RingBuffer::Init(ID3D11Device* device, size_t sizeInBytes, BufferType type) {
    m_bufferSize = sizeInBytes;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = static_cast<UINT>(m_bufferSize);

    // 1. CRÍTICO: Cambiar a DEFAULT para permitir CopyResource
    desc.Usage = D3D11_USAGE_DEFAULT;

    desc.BindFlags = GetBindFlags(type);

    // 2. IMPORTANTE: Quitar los flags de acceso a la CPU
    // Ya no se accede directamente desde la CPU.
    desc.CPUAccessFlags = 0;

    return device->CreateBuffer(&desc, nullptr, m_d3dBuffer.ReleaseAndGetAddressOf());
}
// Implementación de Allocate (El avance atómico del puntero)
RingAllocation RingBuffer::Allocate(size_t sizeInBytes) {
    // Alinear el tamaño. Específico para Constant Buffers (256) pero bueno para todos.
    size_t alignedSize = (sizeInBytes + 255) & ~255;

    size_t oldOffset = m_currentWriteOffset.fetch_add(alignedSize);

    // Comprobación de Wrap-Around (si la nueva asignación excede el tamaño del buffer)
    if (oldOffset + alignedSize > m_bufferSize) {
        // Reiniciar puntero a 0. Esto obliga al siguiente Map a usar DISCARD.
        m_currentWriteOffset.store(alignedSize);
        oldOffset = 0;
    }

    return { oldOffset, alignedSize };
}
