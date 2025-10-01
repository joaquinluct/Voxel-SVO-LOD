#pragma once

#include "FrameStateBase.h"
#include <d3d11.h>
#include <memory>
#include <vector>
#include <wrl/client.h>

// Declaraciones adelantadas para evitar dependencias circulares.
class PipelineOperation;
class DeviceManager;

class RenderFrameState : public FrameStateBase {
public:
    RenderFrameState() = default;

    // Inicializa el contexto diferido para este estado.
    // Llama a esta función al inicio de la aplicación.
    void Initialize(std::shared_ptr<DeviceManager> deviceManager) override;

    //void InitBasicFrameOperations(ID3D11DeviceContext* context, std::vector<std::unique_ptr<PipelineOperation>>& operations);

    // Agrega una operación de renderizado a la lista de comandos.
    // Esta función es llamada por el hilo de escena/actualización.
    void AddOperation(std::unique_ptr<PipelineOperation> operation);
    void AddInitialOperation(std::unique_ptr<PipelineOperation> operation);

    void AddMapUnmapOperation(ID3D11Resource* resource, void* data, size_t dataSize, D3D11_MAP mapType, UINT mapFlags);
    void ExecuteMapUnmapOperations(ID3D11DeviceContext* context);

    // Finaliza la lista de comandos y la prepara para el hilo de renderizado.
    // Llama a esta función una vez que todas las operaciones del frame han sido añadidas.
    ID3D11CommandList* FinalizeCommandList();
    ID3D11CommandList* FinalizeInitialCommandList();

    // Obtiene la lista de comandos finalizada para su ejecución.
    // Esta función es llamada por el hilo de renderizado.
    ID3D11CommandList* GetCommandList() const;
    ID3D11CommandList* GetInitialCommandList() const;

    void SetCommandList(ID3D11CommandList* commandList) {
        m_commandList = commandList;
    }
    void SetInitialCommandList(ID3D11CommandList* commandList) {
        m_initialCommandList = commandList;
    }

    bool HasOperations() const {
        return !m_operations.empty();
    }

    bool HasInitialOperations() const {
        return !m_initialOperations.empty();
    }

private:
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deferredContext;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deferredInitialContext;
    Microsoft::WRL::ComPtr<ID3D11CommandList> m_commandList;
    Microsoft::WRL::ComPtr<ID3D11CommandList> m_initialCommandList;
    std::vector<std::unique_ptr<PipelineOperation>> m_operations;
    std::vector<std::unique_ptr<PipelineOperation>> m_initialOperations;
    std::vector<std::unique_ptr<PipelineOperation>> m_constantBuffersOperations;
};
