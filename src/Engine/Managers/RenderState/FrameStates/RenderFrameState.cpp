// RenderFrameState.cpp
#include "RenderFrameState.h"
#include <Defines/ConcreteOperations.h>
#include <Managers/DeviceManager.h>
#include <memory>
#include <utility>
#include <Windows.h>

void RenderFrameState::Initialize(std::shared_ptr<DeviceManager> deviceManager) {
    // Si el estado no ha sido inicializado y el gestor de dispositivos es válido...
    if (!m_initialized && deviceManager) {
        // ...crea un nuevo contexto diferido para este estado del frame.
        HRESULT hr = deviceManager->GetDevice()->CreateDeferredContext(0, m_deferredContext.GetAddressOf());
        if (SUCCEEDED(hr)) {
            // Marca el estado como inicializado si la creación fue exitosa.
            m_initialized = true;
        }
    }
}

void RenderFrameState::AddOperation(std::unique_ptr<PipelineOperation> operation) {
    // Si el contexto diferido es válido...
    if (m_deferredContext) {
        // ...ejecuta la operación en él para grabar los comandos.
        operation->Execute(m_deferredContext.Get());
        // Almacena la operación en un vector para su gestión.
        m_operations.push_back(std::move(operation));
    }
}

ID3D11CommandList* RenderFrameState::FinalizeCommandList() {
    // Si el contexto diferido es válido...
    if (m_deferredContext) {
        // ...finaliza la lista de comandos a partir del contexto diferido.
        HRESULT hr = m_deferredContext->FinishCommandList(FALSE, m_commandList.GetAddressOf());
        if (SUCCEEDED(hr)) {
            // El contexto diferido ya no es necesario para este frame, así que límpialo.
            m_deferredContext->ClearState();
            // Retorna la lista de comandos finalizada.
            return m_commandList.Get();
        }
    }
    // Retorna nullptr si hubo un error.
    return nullptr;
}

ID3D11CommandList* RenderFrameState::GetCommandList() const {
    // Retorna el puntero a la lista de comandos.
    return m_commandList.Get();
}
