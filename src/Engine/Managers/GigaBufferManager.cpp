#include "GigaBufferManager.h"
#include <cstring>
#include <d3d11.h>
#include <Defines//Context/EngineContext.h>
#include <Locators/Registers/REGISTER_MANAGER_MACRO.h>
#include <ManagerBase.h>
#include <ManagerLocator/ManagerLocator.h>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>
#include <Windows.h>
#include <wrl/client.h>

REGISTER_MANAGER_TYPE(GigaBufferManager, "GigaBufferManager")

GigaBufferManager::GigaBufferManager()
    : m_vertexBufferRing(), m_indexBufferRing(), m_constantBufferRing(),
    m_uploadQueue(), m_queueMutex()
{
}
GigaBufferManager::~GigaBufferManager() {
    // Cleanup if necessary
}

HRESULT GigaBufferManager::InitBuffers(ID3D11Device* device) {
    HRESULT hr = S_OK;

    // Inicialización del Ring Buffer de Vértices (Ej: 16MB)
    hr = m_vertexBufferRing.Init(device, static_cast<size_t>(1024 * 1024) * 16, BufferType::VERTEX);
    if (FAILED(hr)) return hr;

    // Inicialización del Ring Buffer de Índices (Ej: 8MB)
    hr = m_indexBufferRing.Init(device, static_cast<size_t>(1024 * 1024) * 8, BufferType::INDEX);
    if (FAILED(hr)) return hr;

    // Inicialización del Ring Buffer de Constant Buffers (Ej: 1MB, para datos de frame)
    hr = m_constantBufferRing.Init(device, static_cast<size_t>(1024 * 1024) * 1, BufferType::CONSTANT);
    if (FAILED(hr)) return hr;

    return S_OK;
}

HRESULT GigaBufferManager::Init(EngineContext* context) {
    ManagerBase::Init(context);
    auto deviceManager = ManagerLocator::GetDeviceManager();
    if (!deviceManager) {
        return E_FAIL; // Device manager service not available
    }
    m_device = deviceManager->GetDevice();
    if (!m_device) {
        return E_FAIL; // Device not available
    }
    HRESULT hr = InitBuffers(m_device.Get());
    if (FAILED(hr)) {
        return hr; // Failed to initialize buffers
    }

    hr = m_device->CreateDeferredContext(0, m_auxiliaryContext.GetAddressOf());

    //if (FAILED(hr))
    //{
    //    // ¡CRÍTICO! El Contexto Diferido no pudo ser creado.
    //    // Esto suele ocurrir si el driver o la versión de Direct3D no lo soportan.
    //    // Si falló, tu sistema asíncrono no funcionará.
    //    // Debes manejar el error (e.g., lanzar una excepción o fallar la inicialización).
    //}

    return S_OK;
}

// GigaBufferManager.cpp (Función auxiliar)
Microsoft::WRL::ComPtr<ID3D11Buffer> GigaBufferManager::CreateStagingBuffer(size_t byteSize)
{
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = byteSize;
    desc.Usage = D3D11_USAGE_STAGING; // Uso Staging
    desc.BindFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // Acceso de escritura por la CPU
    desc.MiscFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Buffer> stagingBuffer;
    HRESULT hr = m_device->CreateBuffer(&desc, nullptr, stagingBuffer.GetAddressOf());

    if (FAILED(hr)) {
        // Manejo de errores
        return nullptr;
    }
    return stagingBuffer;
}

void GigaBufferManager::ProcessStagingUploads(ID3D11Device* device) {
    // 1. Swap/Lock de la cola de entrada (m_uploadQueue)
    std::queue<UploadRequest> localUploadQueue;
    {
        std::lock_guard<std::mutex> lock(m_uploadMutex); // Usar m_uploadMutex
        std::swap(m_uploadQueue, localUploadQueue);
    }

    std::lock_guard<std::mutex> contextLock(m_contextUsageMutex);

    // 2. Procesamiento (HECHO EN HILO ASÍNCRONO)
    while (!localUploadQueue.empty()) {
        UploadRequest request = std::move(localUploadQueue.front());

        if (!request.StagingBuffer.Get()) {
            // ¡FALLO! El Staging Buffer no se creó.
            // Debes debuguear CreateStagingBuffer para ver por qué falló CreateBuffer.
            localUploadQueue.pop();
            continue;
        }

        D3D11_MAPPED_SUBRESOURCE mapped;

        //Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = m_auxiliaryContext;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = m_context->directX.context;

        // El Hilo de Update mapea el Staging Buffer usando su contexto auxiliar.
        HRESULT hr = context->Map(
            request.StagingBuffer.Get(),
            0,
            D3D11_MAP_WRITE,
            0,
            &mapped
        );

        if (SUCCEEDED(hr)) {
            // 4. COPIA DE DATOS PESADA (Async): ¡Ahora usando los campos correctos!
            memcpy(mapped.pData, request.sourceData, request.byteSize);
            context->Unmap(request.StagingBuffer.Get(), 0);

            // El 'dataGuardian' se destruirá cuando la 'request' salga del scope 
            // de la cola final (m_pendingCopyQueue). 

            // 5. Mover a la cola de copias (para el Render Thread)
            std::lock_guard<std::mutex> copyLock(m_copyMutex);
            m_pendingCopyQueue.push(std::move(request)); // Mueve la petición completa con el Staging Buffer lleno
        }

        localUploadQueue.pop();
    }
}

void GigaBufferManager::ExecutePendingCopies(ID3D11DeviceContext* immediateContext) {
    // 1. Swap/Lock de la cola de copias
    std::queue<UploadRequest> localCopyQueue;
    {
        std::lock_guard<std::mutex> copyLock(m_copyMutex);
        std::swap(m_pendingCopyQueue, localCopyQueue);
    }

    // 2. Ejecutar las copias
    while (!localCopyQueue.empty()) {
        UploadRequest& request = localCopyQueue.front();

        // Comando de COPIA RÁPIDA: Del Staging Buffer (origen) al Ring Buffer (destino)
        immediateContext->CopySubresourceRegion(
            request.targetRing->GetD3DBuffer(), // Destino: Ring Buffer
            0,
            request.allocation.OffsetInBytes,
            0,
            0,
            request.StagingBuffer.Get(), // Origen: Staging Buffer lleno
            0,
            nullptr
        );

        // El Staging Buffer y el Guardián se destruyen al salir del scope/pop.
        localCopyQueue.pop();
    }
}

void GigaBufferManager::Shutdown() {
    // Clear any remaining upload requests
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        while (!m_uploadQueue.empty()) {
            m_uploadQueue.pop();
        }
    }
    ManagerBase::Shutdown();
}
