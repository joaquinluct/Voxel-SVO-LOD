#pragma once

#include <cstdint>
#include <d3d11.h>
#include <Defines//Context/EngineContext.h>
#include <Defines/Structs/RingBuffer.h>
#include <ManagerBase.h>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include <Windows.h>
#include <wrl/client.h>

// -----------------------------------------------------------------------------
// GigaBufferManager: Servicio central para la asignación y la gestión de la cola.
// -----------------------------------------------------------------------------
class GigaBufferManager : public ManagerBase {
public:
    GigaBufferManager();
    ~GigaBufferManager() override;

    HRESULT Init(EngineContext* context) override;
    HRESULT InitBuffers(ID3D11Device* device);
    void Shutdown() override;

    const std::string& GetManagerName() const override {
        static const std::string name = "GigaBufferManager"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
        return name;
    }
    static const std::string& GetStaticManagerName()
    {
        static const std::string name = "GigaBufferManager"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
        return name;
    }


    template<typename T>
    RingAllocation QueueUpload(
        RingBuffer* targetRing,
        std::shared_ptr<const std::vector<T>> sharedDataBuffer)
    {
        size_t elementCount = sharedDataBuffer->size();
        size_t byteSize = elementCount * sizeof(T);

        // Asegúrate de que el tamaño no sea cero antes de llamar a CreateStagingBuffer
        if (byteSize == 0) {
            // Podrías devolver una asignación nula y saltarte el encolado
            return { 0, 0 };
        }

        // 1. Asignar espacio en el Ring Buffer de destino
        RingAllocation allocation = targetRing->Allocate(byteSize);

        // 2. Crear el Staging Buffer. 
        // Creamos aquí el recurso D3D11, lo cual es generalmente seguro 
        // ya que ID3D11Device::CreateBuffer es a menudo Thread-Safe.
        Microsoft::WRL::ComPtr<ID3D11Buffer> stagingBuffer = CreateStagingBuffer(byteSize);

        // 3. Crear el Guardián de Vida y la Petición
        std::shared_ptr<const void> guardian =
            std::static_pointer_cast<const void>(sharedDataBuffer);

        // Creamos la petición.
        UploadRequest request = {
            targetRing,
            allocation,
            byteSize,
            (const void*)sharedDataBuffer->data(), // sourceData
            guardian,
            stagingBuffer  // El Staging Buffer (ahora en la posición correcta de la struct)
        };

        // 4. Encolar la petición de forma segura
        {
            // USAR EL MUTEX CORRECTO: m_uploadMutex (o m_queueMutex si lo mantienes así)
            std::lock_guard<std::mutex> lock(m_uploadMutex);
            m_uploadQueue.push(std::move(request));
        }

        return allocation;
    }


    // Obtener la instancia del RingBuffer (EXISTENTE)
    RingBuffer* GetVertexBufferRing() { return &m_vertexBufferRing; }
    RingBuffer* GetIndexBufferRing() { return &m_indexBufferRing; }
    RingBuffer* GetConstantBufferRing() { return &m_constantBufferRing; }

    Microsoft::WRL::ComPtr<ID3D11Buffer> CreateStagingBuffer(size_t byteSize);
    void ProcessStagingUploads(ID3D11Device* device);
    void ExecutePendingCopies(ID3D11DeviceContext* immediateContext);

private:
    RingBuffer m_vertexBufferRing;
    RingBuffer m_indexBufferRing;
    RingBuffer m_constantBufferRing;

    // Cola de peticiones hilo-segura
    // COLAS DE TRABAJO (Upload Queue y Copy Queue)
    // La cola que los Jobs llenan (El Hilo de Update la vacía)
    std::queue<UploadRequest> m_uploadQueue;
    std::mutex m_queueMutex;

    // CONTEXTOS
    // Si usas un Contexto Diferido para el Hilo de Update (¡la mejor opción!):
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_auxiliaryContext;
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    // O si usas un Contexto Auxiliar propio del hilo (menos canónico).


    // La cola que el Hilo de Update llena con Staging Buffers listos (El Hilo de Render la vacía)
    std::queue<UploadRequest> m_pendingCopyQueue;

    // MUTEXES
    // Mutex para proteger el acceso a m_uploadQueue
    std::mutex m_uploadMutex;

    // Mutex para proteger el acceso a m_pendingCopyQueue
    std::mutex m_copyMutex;

    std::mutex m_contextUsageMutex;
};
