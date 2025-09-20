#pragma once

#include "FrameStateBase.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <memory>

// Declaraciones adelantadas para evitar dependencias circulares.
class PipelineOperation;
class DeviceManager;

class RenderFrameState : public FrameStateBase {
public:
    RenderFrameState() = default;

    // Inicializa el contexto diferido para este estado.
    // Llama a esta función al inicio de la aplicación.
    void Initialize(std::shared_ptr<DeviceManager> deviceManager);

    // Agrega una operación de renderizado a la lista de comandos.
    // Esta función es llamada por el hilo de escena/actualización.
    void AddOperation(std::unique_ptr<PipelineOperation> operation);

    // Finaliza la lista de comandos y la prepara para el hilo de renderizado.
    // Llama a esta función una vez que todas las operaciones del frame han sido añadidas.
    ID3D11CommandList* FinalizeCommandList();

    // Obtiene la lista de comandos finalizada para su ejecución.
    // Esta función es llamada por el hilo de renderizado.
    ID3D11CommandList* GetCommandList() const;

private:
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deferredContext;
    Microsoft::WRL::ComPtr<ID3D11CommandList> m_commandList;
    std::vector<std::unique_ptr<PipelineOperation>> m_operations;
};
