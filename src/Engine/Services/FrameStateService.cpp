#include "stdafx.h"
#include "FrameStateService.h"
#include <../Includes/FrameStates.h>
#include <Defines/Contants/FrameState.h>
#include <Defines/FrameStateDefinition.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <Locators/Registers/REGISTER_SERVICE_MACRO.h>
#include <mutex>
#include <vector>

REGISTER_SERVICE_TYPE(FrameStateService, "FrameStateService")

FrameStateService::FrameStateService() {
}

FrameStateService::~FrameStateService() {

}

HRESULT FrameStateService::Init() {

    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (!m_deviceManager) {
        return E_FAIL;
    }
    // Inicialización de todos los buffers de estado
    // Esto asegura que todos los estados estén disponibles desde el inicio
    /*GetBuffer(FRAME_STATE_CAMERA);
    GetBuffer(FRAME_STATE_COMMON);
    GetBuffer(FRAME_STATE_LIGHTING);
    GetBuffer(FRAME_STATE_MATERIAL);
    GetBuffer(FRAME_STATE_SKYBOX);
    GetBuffer(FRAME_STATE_TERRAIN);
    GetBuffer(FRAME_STATE_WATER);
    GetBuffer(FRAME_STATE_TIME);
    GetBuffer(FRAME_STATE_MESH);
    GetBuffer(FRAME_STATE_DEVICE);
    GetBuffer(FRAME_STATE_PASS);
    GetBuffer(FRAME_STATE_VIEWPORT);
    GetBuffer(FRAME_STATE_SHADER);
    GetBuffer(FRAME_STATE_PIPELINE);
    GetBuffer(FRAME_STATE_RENDER);*/

    return S_OK;
}

// ---------------------------------------------------------------------------------------------------------------
// Acceso a los diferentes estados del frame mediante plantillas para evitar la repetición de código.
// Cada método específico llama a GetData con el nombre del estado correspondiente.
// ---------------------------------------------------------------------------------------------------------------
CameraFrameState* FrameStateService::CameraState(bool FromReadBuffer) {
    return GetData<CameraFrameState>(FRAME_STATE_CAMERA, FromReadBuffer);
}
CommonFrameState* FrameStateService::CommonState(bool FromReadBuffer) {
    return GetData<CommonFrameState>(FRAME_STATE_COMMON, FromReadBuffer);
}
LightFrameState* FrameStateService::LightingState(bool FromReadBuffer) {
    return GetData<LightFrameState>(FRAME_STATE_LIGHTING, FromReadBuffer);
}
MaterialFrameState* FrameStateService::MaterialState(bool FromReadBuffer) {
    return GetData<MaterialFrameState>(FRAME_STATE_MATERIAL, FromReadBuffer);
}
SkyboxFrameState* FrameStateService::SkyboxState(bool FromReadBuffer) {
    return GetData<SkyboxFrameState>(FRAME_STATE_SKYBOX, FromReadBuffer);
}
TerrainFrameState* FrameStateService::TerrainState(bool FromReadBuffer) {
    return GetData<TerrainFrameState>(FRAME_STATE_TERRAIN, FromReadBuffer);
}
WaterFrameState* FrameStateService::WaterState(bool FromReadBuffer) {
    return GetData<WaterFrameState>(FRAME_STATE_WATER, FromReadBuffer);
}
TimeFrameState* FrameStateService::TimeState(bool FromReadBuffer) {
    return GetData<TimeFrameState>(FRAME_STATE_TIME, FromReadBuffer);
}
//MeshFrameState* FrameStateService::MeshState(bool FromReadBuffer) {
//    return GetData<MeshFrameState>(FRAME_STATE_MESH, FromReadBuffer);
//}
//DeviceFrameState* FrameStateService::DeviceState(bool FromReadBuffer) {
//    return GetData<DeviceFrameState>(FRAME_STATE_DEVICE, FromReadBuffer);
//}
PassFrameState* FrameStateService::PassState(bool FromReadBuffer) {
    return GetData<PassFrameState>(FRAME_STATE_PASS, FromReadBuffer);
}
ConstantsBufferFrameState* FrameStateService::ConstantBuffersState(bool FromReadBuffer) {
    return GetData<ConstantsBufferFrameState>(FRAME_STATE_CONSTANT_BUFFERS, FromReadBuffer);
}
ShaderFrameState* FrameStateService::ShaderState(bool FromReadBuffer) {
    return GetData<ShaderFrameState>(FRAME_STATE_SHADER, FromReadBuffer);
}
PipelineFrameState* FrameStateService::PipelineState(bool FromReadBuffer) {
    return GetData<PipelineFrameState>(FRAME_STATE_PIPELINE, FromReadBuffer);
}
RenderFrameState* FrameStateService::RenderState(bool FromReadBuffer) {
    return GetData<RenderFrameState>(FRAME_STATE_RENDER, FromReadBuffer);
}
MeshesFrameState* FrameStateService::MeshesState(bool FromReadBuffer) {
    return GetData<MeshesFrameState>(FRAME_STATE_MESHES, FromReadBuffer);
}

// ---------------------------------------------------------------------------------------------------------------
// GetWriteState
// GetReadState
//	Obtienen el estado actual de escritura y lectura del buffer, respectivamente.
//	Utilizan índices atómicos para asegurar la coherencia en entornos multihilo.
// ---------------------------------------------------------------------------------------------------------------
FrameStateBase* FrameStateService::GetWriteState(FrameStateBuffer* buffer) {
    int index = buffer->writeIndex.load();
    return buffer->buffers[index];
}
FrameStateBase* FrameStateService::GetReadState(FrameStateBuffer* buffer) {
    int index = buffer->readIndex.load();
    return buffer->buffers[index];
}

// ---------------------------------------------------------------------------------------------------------------
// GetBuffer
//	Obtiene el buffer asociado a un nombre de estado específico.
//	Si el buffer no existe, lo crea y lo almacena en el mapa de datos.
// ---------------------------------------------------------------------------------------------------------------
FrameStateBuffer* FrameStateService::GetBuffer(std::string_view stateName) {

    FrameStateBuffer* buffer = nullptr;

    const auto& state = m_data.find(stateName);

    if (state == m_data.end()) {
        const auto& newState = CreateState(stateName);
        buffer = newState.get();
    }
    else {
        buffer = state->second.get();
    }

    return buffer;
}

// ---------------------------------------------------------------------------------------------------------------
// SetData
// Actualiza el estado de un buffer específico con nuevos datos.
// ---------------------------------------------------------------------------------------------------------------
void FrameStateService::SetData(std::string_view stateName, FrameStateBase* data) {
    FrameStateBuffer* buffer = GetBuffer(stateName);
    if (!buffer) {
        return;
    }
    //buffer->mutex.lock();
    FrameStateBase* state = GetWriteState(buffer);
    state = static_cast<FrameStateBase*>(data);
    //buffer->mutex.unlock();
}

// ---------------------------------------------------------------------------------------------------------------
// CreateState
//	Crea un nuevo FrameStateBuffer para un nombre de estado específico
//	y lo almacena en el mapa de datos.
// ---------------------------------------------------------------------------------------------------------------
std::shared_ptr<FrameStateBuffer> FrameStateService::CreateState(std::string_view stateName) {
    auto threadData = std::make_shared<FrameStateBuffer>();
    threadData->buffers[0] = {};
    threadData->buffers[1] = {};

    m_data[stateName] = threadData;
    return threadData;
}

// -----------------------------------------------------------------------------------------------
// SwapBuffers
//	Intercambia los índices de lectura y escritura para todos los estados,
//	permitiendo que los datos escritos en el buffer de escritura se lean
//  en el siguiente ciclo de renderizado.
// -----------------------------------------------------------------------------------------------

// Este método intercambia el valor de los índices del nombre del bufer pasado.
void FrameStateService::SwapBuffer(std::string_view stateName) {
    const auto& state = m_data.find(stateName);
    if (state != m_data.end()) {
        auto& buffer = state->second;
        buffer->mutex.lock();
        int writeIndex = buffer->writeIndex.load();
        int readIndex = buffer->readIndex.load();
        buffer->writeIndex.store(readIndex);
        buffer->readIndex.store(writeIndex);
        buffer->mutex.unlock();
    }
}

// Este método intercambia el valor de los índices de todos los búferes
void FrameStateService::SwapBuffers() {
    for (auto& [key, buffer] : m_data) {
        SwapBuffer(key);
    }
}

// Este método pasa los valores reales de los búferes de escritura
// de cada uno a de lectura.
void FrameStateService::SwapBufferContent(std::string_view stateName) {
    const auto& state = m_data.find(stateName);
    if (state != m_data.end()) {
        auto& buffer = state->second;
        buffer->mutex.lock();
        int writeIndex = buffer->writeIndex.load();
        int readIndex = buffer->readIndex.load();
        buffer->buffers[readIndex] = buffer->buffers[writeIndex];
        buffer->mutex.unlock();
    }
}

void FrameStateService::SwapBuffersContent() {
    for (auto& [key, buffer] : m_data) {
        SwapBufferContent(key);
    }
}

// ---------------------------------------------------------------------------------------------------------------
// LockAll
// Bloquea todos los buffers para evitar que se realicen operaciones de lectura o escritura
// ---------------------------------------------------------------------------------------------------------------
//void FrameStateService::LockAll() {
//    for (auto& [key, buffer] : m_data) {
//        std::lock_guard<std::mutex> lock(buffer->mutex);
//        //buffer->mutex.lock();
//    }
//}

std::vector<std::unique_lock<std::mutex>> FrameStateService::LockAll() {
    std::vector<std::unique_lock<std::mutex>> locks;
    for (auto& [key, buffer] : m_data) {
        locks.emplace_back(buffer->mutex);
    }
    return locks;
}

void FrameStateService::UnlockAll(std::vector<std::unique_lock<std::mutex>>& locks) {
    for (auto& lock : locks) {
        if (lock.owns_lock()) {
            lock.unlock();
        }
    }
    locks.clear();
}

//std::unique_lock<std::mutex> FrameStateService::LockRenderState() {
//    // Obtenemos una referencia al mutex del buffer de renderizado.
//    auto& renderMutex = m_data[FRAME_STATE_RENDER]->mutex;
//
//    // Creamos un unique_lock que bloquea el mutex inmediatamente.
//    // El lock se mantendrá activo hasta que se destruya o se libere explícitamente.
//    std::unique_lock<std::mutex> lock(renderMutex);
//
//    // El 'lock' se devuelve. Gracias a la optimización de retorno de C++ (RVO/NRVO),
//    // la propiedad del candado se mueve eficientemente al llamador,
//    // sin que el mutex se libere.
//    return lock;
//}


// ---------------------------------------------------------------------------------------------------------------
// UnlockAll
// Desbloquea todos los buffers para permitir operaciones de lectura o escritura
// ---------------------------------------------------------------------------------------------------------------
void FrameStateService::UnlockAll() {
    for (auto& [key, buffer] : m_data) {
        buffer->mutex.unlock();
    }
}

// ---------------------------------------------------------------------------------------------------------------
// Lock
// Bloquea el buffer de un estado específico para evitar que se realicen operaciones de lectura o escritura
// ---------------------------------------------------------------------------------------------------------------
void FrameStateService::Lock(std::string_view stateName) {
    const auto& state = m_data.find(stateName);
    if (state != m_data.end()) {
        state->second->mutex.lock();
    }
}
// ---------------------------------------------------------------------------------------------------------------
// Unlock
// Desbloquea el buffer de un estado específico para permitir operaciones de lectura o escritura
// ---------------------------------------------------------------------------------------------------------------
void FrameStateService::Unlock(std::string_view stateName) {
    const auto& state = m_data.find(stateName);
    if (state != m_data.end()) {
        state->second->mutex.unlock();
    }
}

bool FrameStateService::TryLock(std::string_view stateName) {
    const auto& state = m_data.find(stateName);
    if (state != m_data.end()) {
        return state->second->mutex.try_lock();
    }
    return true;
}

// Control del Rendering Stateç
// ---------------------------------------------------------------------------------------------------------------
void FrameStateService::BeginRendering() {
    m_isRendering = true;
}
void FrameStateService::EndRendering() {
    m_isRendering = false;
}

// ---------------------------------------------------------------------------------------------------------------
// Shutdown
//	Limpia los recursos asociados al servicio.
// ---------------------------------------------------------------------------------------------------------------
void FrameStateService::Shutdown() {
    m_data.clear();
}
