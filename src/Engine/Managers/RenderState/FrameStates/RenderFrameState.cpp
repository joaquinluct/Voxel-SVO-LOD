// RenderFrameState.cpp
#include "RenderFrameState.h"
//#include <Defines/ConcreteOperations.h>
#include <d3d11.h>
#include <Managers/RenderManager/Pipeline/ConcreteOperations.h>
#include <Managers/DeviceManager.h>
#include <memory>
#include <utility>
#include <Windows.h>

void RenderFrameState::Initialize(std::shared_ptr<DeviceManager> deviceManager) {
    // Si el estado no ha sido inicializado y el gestor de dispositivos es válido...
    if (deviceManager) {
        // ...crea un nuevo contexto diferido para este estado del frame.
        /*HRESULT hr = deviceManager->GetDevice()->CreateDeferredContext(0, m_deferredInitialContext.GetAddressOf());
        if (FAILED(hr)) {
            OutputDebugStringA("Error Inicialización del Contexto Diferido inicial.");
        }*/
        HRESULT hr = deviceManager->GetDevice()->CreateDeferredContext(0, m_deferredContext.GetAddressOf());
        if (FAILED(hr)) {
            OutputDebugStringA("Error Inicialización del Contexto Diferido principal.");
        }
    }
}

void RenderFrameState::AddInitialOperation(std::unique_ptr<PipelineOperation> operation) {
    // Si el contexto diferido es válido...
    if (m_deferredInitialContext) {
        // ...ejecuta la operación en él para grabar los comandos.
        operation->Execute(m_deferredInitialContext.Get());
        // Almacena la operación en un vector para su gestión.
        m_initialOperations.push_back(std::move(operation));
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

void RenderFrameState::AddMapUnmapOperation(ID3D11Resource* resource, void* data, size_t dataSize, D3D11_MAP mapType, UINT mapFlags)
{
    //if (m_deferredContext && resource && data && dataSize > 0) {
    //    // Crea una nueva operación de mapeo/desmapeo y la añade a la lista de operaciones.
    //    auto mapUnmapOp = std::make_unique<MapUnmapOperation>(resource, data, dataSize);
    //    //mapUnmapOp->Execute(m_deferredContext.Get());
    //    m_constantBuffersOperations.push_back(std::move(mapUnmapOp));
    //}
}

void RenderFrameState::ExecuteMapUnmapOperations(ID3D11DeviceContext* context)
{
    if (m_deferredContext) {
        for (const auto& op : m_operations) {
            // Ejecuta solo las operaciones de mapeo/desmapeo.
            if (dynamic_cast<MapUnmapOperation*>(op.get())) {
                op->Execute(context);
            }
        }
    }
}

ID3D11CommandList* RenderFrameState::FinalizeCommandList() {
    // Si el contexto diferido es válido...
    if (m_deferredContext) {
        // ...finaliza la lista de comandos a partir del contexto diferido.
        HRESULT hr = m_deferredContext->FinishCommandList(FALSE, m_commandList.ReleaseAndGetAddressOf());
        if (SUCCEEDED(hr)) {
            // El contexto diferido ya no es necesario para este frame, así que límpialo.
            m_deferredContext.Reset();
            //m_deferredContext->ClearState();
            //m_deferredContext->Release();
            // Retorna la lista de comandos finalizada.
            m_operations.clear();
            return m_commandList.Get();
        }
    }
    // Retorna nullptr si hubo un error.
    return nullptr;
}

ID3D11CommandList* RenderFrameState::FinalizeInitialCommandList() {
    // Si el contexto diferido es válido...
    if (m_deferredInitialContext) {
        // ...finaliza la lista de comandos a partir del contexto diferido.
        HRESULT hr = m_deferredInitialContext->FinishCommandList(FALSE, m_initialCommandList.ReleaseAndGetAddressOf());
        if (SUCCEEDED(hr)) {
            // El contexto diferido ya no es necesario para este frame, así que límpialo.
            m_deferredInitialContext.Reset();
            //m_deferredInitialContext->Release();
            // Retorna la lista de comandos finalizada.
            m_initialOperations.clear();
            return m_initialCommandList.Get();
        }
    }
    // Retorna nullptr si hubo un error.
    return nullptr;
}

ID3D11CommandList* RenderFrameState::GetCommandList() const {
    // Retorna el puntero a la lista de comandos.
    return m_commandList.Get();
}

ID3D11CommandList* RenderFrameState::GetInitialCommandList() const {
    // Retorna el puntero a la lista de comandos.
    return m_initialCommandList.Get();
}
