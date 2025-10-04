#pragma once

#include <atomic>
#include <cstdint>
#include <d3d11.h>
#include <memory>
#include <vector>
#include <Windows.h>
#include <wrl/client.h>


// -----------------------------------------------------------------------------
// RingAllocation: Rango de memoria asignado.
// -----------------------------------------------------------------------------
struct RingAllocation {
    size_t OffsetInBytes = 0;
    size_t SizeInBytes = 0;
};

// -----------------------------------------------------------------------------
// BufferType: Tipo de recurso para la creación.
// -----------------------------------------------------------------------------
enum class BufferType {
    VERTEX,
    INDEX,
    CONSTANT
};

// -----------------------------------------------------------------------------
// UploadRequest: Paquete de trabajo para la cola de subida.
// -----------------------------------------------------------------------------
struct UploadRequest {
    class RingBuffer* targetRing;
    RingAllocation allocation;

    size_t byteSize;

    // ¡CRÍTICO! El puntero a los datos de la CPU (el std::vector<T>::data())
    const void* sourceData;

    // El guardián de vida (mantiene vivo el std::vector hasta el Map/Unmap)
    std::shared_ptr<const void> dataGuardian;

    // El buffer que usa el Hilo de Update
    Microsoft::WRL::ComPtr<ID3D11Buffer> StagingBuffer;
};

// -----------------------------------------------------------------------------
// RingBuffer: Clase central que gestiona el buffer D3D11 y la asignación.
// -----------------------------------------------------------------------------
class RingBuffer {
public:
    RingBuffer() : m_currentWriteOffset(0), m_bufferSize(0) {}

    HRESULT Init(ID3D11Device* device, size_t sizeInBytes, BufferType type);

    RingAllocation Allocate(size_t sizeInBytes);

    ID3D11Buffer* GetD3DBuffer() const { return m_d3dBuffer.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_d3dBuffer;
    std::atomic<size_t> m_currentWriteOffset;
    size_t m_bufferSize;

    D3D11_BIND_FLAG GetBindFlags(BufferType type) const;
};
