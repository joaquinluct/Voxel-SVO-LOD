#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <cstddef>

// Pequeña implementación plantilla de un Ring Buffer para instancias.
// Esta clase está pensada como plantilla inicial; ajusta la integración
// con el resto del motor según las convenciones del proyecto.
class InstancingRingBuffer {
public:
    InstancingRingBuffer();
    ~InstancingRingBuffer();

    // Inicializa el buffer con la capacidad máxima de instancias y el tamaño en bytes
    // de cada instancia (por ejemplo, una matriz World + float LOD).
    HRESULT Init(ID3D11Device* device, std::size_t maxInstances, std::size_t instanceSize);

    // Actualiza los datos de instancias (escribe `numInstances` elementos desde `data`).
    // Implementación simple que sobrescribe desde el inicio cuando se llena.
    void UpdateInstances(ID3D11DeviceContext* context, const void* data, std::size_t numInstances);

    // Vincula el buffer de instancias al pipeline de entrada (slot por defecto 1).
    void Bind(ID3D11DeviceContext* context, unsigned int startSlot = 1);

    // Liberar recursos.
    void Shutdown();

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBuffer;
    std::size_t m_maxInstances = 0;
    std::size_t m_instanceSize = 0;
    std::size_t m_currentOffset = 0; // en bytes
};
