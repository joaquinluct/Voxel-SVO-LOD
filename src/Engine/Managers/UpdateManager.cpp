#include "UpdateManager.h"
#include <../Includes/FrameStates.h>
#include <Assets/Base/MeshAsset.h>
#include <CameraManager.h>
#include <cstdlib>
#include <Defines/Contants/FrameState.h>
#include <functional>
#include <Game/Systems/Lighting.h>
#include <Game/Systems/Skybox.h>
#include <Game/Systems/Water.h>
#include <Game/Systems/World.h>
#include <Locators/Registers/REGISTER_MANAGER_MACRO.h>
#include <ManagerLocator/ManagerLocator.h>
#include <map>
#include <memory>
#include <SceneManager.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Services/ThreadPool.h>
#include <utility>

REGISTER_MANAGER_TYPE(UpdateManager, "UpdateManager")

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
UpdateManager::UpdateManager() :
    m_futures(), m_threadPool(nullptr), m_cameraManager(nullptr), m_shaderManager(nullptr),
    m_frameStateService(nullptr), m_lighting(nullptr), m_world(nullptr), m_water(nullptr),
    m_mesh(nullptr), m_skybox(nullptr), m_sceneManager(nullptr)
{
}
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
UpdateManager::~UpdateManager()
{
}

// ---------------------------------------------------------------------------
// RunLoop del UpdateManager
// ---------------------------------------------------------------------------
void UpdateManager::RunLoop()
{
    // NOTE: Migrated to AAA threading model. UpdateManager no longer runs
    // a dedicated thread. Update() must be called from the Engine main loop
    // (Engine::MainLoop) on the main thread. Keep RunLoop() as a no-op to
    // preserve compatibility with ManagerBase/ThreadedService lifecycle.
    (void)m_running;
    return;

}

// ----------------------------------------------------------------------------
// Inicialización del UpdateManager
// ----------------------------------------------------------------------------
HRESULT UpdateManager::Init(EngineContext* context)
{
    ManagerBase::Init(context);

    m_threadPool = ServiceLocator::GetService<ThreadPool>();
    if (!m_threadPool) {
        return E_FAIL;
    }
    m_cameraManager = ManagerLocator::GetCameraManager();
    if (!m_cameraManager)
    {
        return E_FAIL;
    }
    m_shaderManager = ManagerLocator::GetShaderManager();
    if (!m_shaderManager)
    {
        return E_FAIL;
    }
    m_frameStateService = ServiceLocator::GetService<FrameStateService>();
    if (!m_frameStateService)
    {
        return E_FAIL;
    }
    m_lighting = ServiceLocator::GetService<Lighting>();
    if (!m_lighting)
    {
        return E_FAIL;
    }
    m_world = ServiceLocator::GetService<World>();
    if (!m_world)
    {
        return E_FAIL;
    }
    m_water = ServiceLocator::GetService<Water>();
    if (!m_water)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT UpdateManager::PostInit()
{
    m_sceneManager = ManagerLocator::GetManager<SceneManager>();
    if (!m_sceneManager) {
        return E_FAIL;
    }
    return S_OK;
}

// ----------------------------------------------------------------------------
// Inicialización de la tareas de actualización en paralelo
// ----------------------------------------------------------------------------
FutureUpdateJob UpdateManager::AddUpdateJob(const std::string& name, std::function<bool()> task, bool allowDuplicates) {
    auto cancel_token = std::make_shared<std::atomic<bool>>(false);

    auto wrappedTask = [task, name, allowDuplicates, cancel_token]() -> UpdateJob {
        UpdateJob job;
        job.name = name;
        job.allowDuplicates = allowDuplicates;
        job.startTime = std::chrono::high_resolution_clock::now();
        job.cancel_token = cancel_token;
        job.isSuccessful = false; // Valor por defecto

        if (!cancel_token->load()) {
            job.isSuccessful = task(); // CAMBIO: Asigna el resultado de la tarea
        }
        return job;
        };

    FutureUpdateJob future = m_threadPool->enqueue(wrappedTask); // ← devuelve FutureUpdateJob
    std::lock_guard<std::mutex> lock(m_mutex);
    m_futures[std::rand()] = std::move(future);

    return future;
}

void IsJobCompleted(std::string jobName) {

}

//void UpdateManager::StartThreads(float deltaTime)
//{
//    m_futures.push_back(m_threadPool->enqueue(&UpdateManager::UpdateMainData, this, deltaTime));
//    m_futures.push_back(m_threadPool->enqueue(&UpdateManager::UpdateTerrainData, this, deltaTime));
//}

// ----------------------------------------------------------------------------
// Funciones de actualización de los estados
// ----------------------------------------------------------------------------
FrameStateBase* UpdateManager::UpdateTerrainState(float deltaTime) {
    TerrainFrameState* terrain = m_frameStateService->TerrainState(false);
    //terrain->SetChunks(m_world->GetTerrain()->GetChunks(m_cameraManager->GetCurrentCamera()));
    return terrain;
}
FrameStateBase* UpdateManager::UpdateCameraState(float deltaTime) {
    CameraFrameState* camera = m_frameStateService->CameraState(false);
    camera->SetCameraPosition(m_cameraManager->GetCurrentCameraPosition());
    camera->SetViewMatrix(m_cameraManager->GetCurrentViewMatrix(true));
    camera->SetProjectionMatrix(m_cameraManager->GetCurrentProjectionMatrix(true));
    return camera;
}
FrameStateBase* UpdateManager::UpdateLightState(float deltaTime) {
    LightFrameState* light = m_frameStateService->LightingState(false);
    light->SetLightDirection(m_lighting->GetLightDirection());
    light->SetLightColor(m_lighting->GetLightColor());
    return light;
}
FrameStateBase* UpdateManager::UpdateWaterState(float deltaTime) {
    WaterFrameState* water = m_frameStateService->WaterState(false);
    water->SetWaterData(WaterDefinition::GetTemporalWaterData());
    water->SetWaterTileTransformMatrices(m_water->GetWaterMatrices());
    return water;
}
FrameStateBase* UpdateManager::UpdateCommonState(float deltaTime) {
    CommonFrameState* common = m_frameStateService->CommonState(false);
    //common->SetWorldMatrix(this->GetMesh()->GetWorldMatrix());
    common->SetWorldMatrix(DirectX::XMMatrixIdentity());
    return common;
}
FrameStateBase* UpdateManager::UpdateMaterialState(float deltaTime) {
    const MeshAsset* mesh = this->GetMesh();
    if (!mesh || !mesh->GetMaterial()) {
        return nullptr;
    }
    MaterialFrameState* material = m_frameStateService->MaterialState(false);
    material->SetConstantBuffers(this->GetMesh()->GetMaterial()->GetConstantBuffers());
    material->SetNumTextures(this->GetMesh()->GetMaterial()->GetNumTextures());
    material->SetTextureTransform(this->GetMesh()->GetMaterial()->GetTextureTranforms());
    return material;
}
FrameStateBase* UpdateManager::UpdateSkyboxState(float deltaTime) {
    SkyboxFrameState* skyBox = m_frameStateService->SkyboxState(false);
    if (!m_skybox) {
        return nullptr;
    }
    skyBox->SetSkyColor(m_skybox->GetSkyColor());
    skyBox->SetSunColor(m_skybox->GetSunColor());
    return skyBox;
}
FrameStateBase* UpdateManager::UpdateTimeState(float deltaTime) {
    TimeFrameState* time = m_frameStateService->TimeState(false);
    return time;
}
//FrameStateBase* UpdateManager::UpdateMeshState(float deltaTime) {
//    // Estamos al final del Update de todos los servicios y managers, por lo que el mesh debe ser reseteado.
//    MeshFrameState* mesh = m_frameStateService->MeshState(false);
//    
//    return mesh;
//}
FrameStateBase* UpdateManager::UpdateDeviceState(float deltaTime) {
    //DeviceFrameState* device = m_frameStateService->DeviceState(false);
    //device->SetClearColor({});
    //return device;
    return nullptr;
}
FrameStateBase* UpdateManager::UpdatePassState(float deltaTime) {
    PassFrameState* pass = m_frameStateService->PassState(false);
    return pass;
}
FrameStateBase* UpdateManager::UpdateConstantBuffersState(float deltaTime) {
    ConstantsBufferFrameState* cBuffer = m_frameStateService->ConstantBuffersState(false);
    /*if (!cBuffer->IsInitialized()) {
        viewport->SetViewports(m_frameStateService->PipelineState(false)->GetAllViewports());
    }*/
    return cBuffer;
}
FrameStateBase* UpdateManager::UpdateShaderState(float deltaTime) {
    // Estamos al final del Update de todos los servicios y managers, por lo que el shader debe ser re-seteado.    
    ShaderFrameState* shaderState = m_frameStateService->ShaderState(false);
    shaderState->ClearMatrices();
    shaderState->SetShaderName("");
    return shaderState;
}
FrameStateBase* UpdateManager::UpdatePipelineState(float deltaTime) {
    PipelineFrameState* pipeline = m_frameStateService->PipelineState(false);
    return pipeline;
}

// ----------------------------------------------------------------------------
// Gestión del Mesh actual
// ----------------------------------------------------------------------------
//void UpdateManager::SetMesh(const std::shared_ptr<MeshAsset>& mesh) {
//    MeshFrameState* meshState = m_frameStateService->MeshState();
//    meshState->AddMesh(mesh.get());
//    m_frameStateService->SetData(FRAME_STATE_MESH, meshState);
//    //m_frameStateService->FlushAndFreeze(FRAME_STATE_MESH);
//    m_mesh = mesh;
//}

// ----------------------------------------------------------------------------
// Resets de los estados
// Normalmente al final del Update:
//     Unos actualizan con nuevos datos en este momento.
//     Otros resetean su estado para que no queden datos de un frame a otro.
// ----------------------------------------------------------------------------
//FrameStateBase* UpdateManager::ResetMeshState(float deltaTime) {
//    MeshFrameState* mesh = m_frameStateService->MeshState(false);
//    //mesh->SetMesh(nullptr);
//    return mesh;
//}
FrameStateBase* UpdateManager::ResetShaderState(float deltaTime) {
    ShaderFrameState* shaderState = m_frameStateService->ShaderState(false);
    shaderState->ClearMatrices();
    shaderState->SetShaderName("");
    return shaderState;
}

// ----------------------------------------------------------------------------
// TAREA DE ACTUALIZACIÓN: Main
// ----------------------------------------------------------------------------
UpdatedJobs UpdateManager::UpdateMainData(float deltaTime) {

    UpdatedJobs updatedStates{};

    // ACTUALIZAR ESTADOS QUE LO REQUIEREN
    FrameStateBase* state = UpdatePassState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_PASS, state);
        //updatedStates.push_back(FRAME_STATE_PASS.data());
    }
    state = UpdateCameraState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_CAMERA, state);
        //updatedStates.push_back(FRAME_STATE_CAMERA.data());
    }
    state = UpdateLightState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_LIGHTING, state);
        //updatedStates.push_back(FRAME_STATE_LIGHTING.data());
    }
    state = UpdateWaterState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_WATER, state);
        //updatedStates.push_back(FRAME_STATE_WATER.data());
    }
    state = UpdateCommonState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_COMMON, state);
        //updatedStates.push_back(FRAME_STATE_COMMON.data());
    }
    state = UpdateTimeState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_TIME, state);
        //updatedStates.push_back(FRAME_STATE_TIME.data());
    }
    state = UpdatePassState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_PASS, state);
        //updatedStates.push_back(FRAME_STATE_PASS.data());
    }
    state = UpdateMaterialState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_MATERIAL, state);
        //updatedStates.push_back(FRAME_STATE_MATERIAL.data());
    }
    state = UpdateSkyboxState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_SKYBOX, state);
        //updatedStates.push_back(FRAME_STATE_SKYBOX.data());
    }
    state = UpdateDeviceState(deltaTime);
    if (state) {
        //m_frameStateService->SetData(FRAME_STATE_DEVICE, state);
        //updatedStates.push_back(FRAME_STATE_DEVICE.data());
    }
    return updatedStates;
}

// ----------------------------------------------------------------------------
// TAREA DE ACTUALIZACIÓN: Terreno
// ----------------------------------------------------------------------------
UpdatedJobs UpdateManager::UpdateTerrainData(float deltaTime) {
    FrameStateBase* state = UpdateTerrainState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_TERRAIN, state);
    }
    return UpdatedJobs{}; //{ FRAME_STATE_TERRAIN.data()};
}

void UpdateManager::Update(float deltaTime) {

    // Ejecutar las actualizaciones sincronas necesarias en el hilo principal
    // (antes estas podían residir en tareas dedicadas). Estas funciones
    // actualizan los frame states y llaman a FrameStateService::SetData.
    UpdatedJobs mainUpdated = UpdateMainData(deltaTime);
    UpdatedJobs terrainUpdated = UpdateTerrainData(deltaTime);

    // Procesar futuros completados en segundo plano (si los hay)
    std::map<int, FutureUpdateJob> finishedJobs;
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_futures.begin(); it != m_futures.end(); ) {
        auto& future = it->second;
        if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            finishedJobs.emplace(it->first, std::move(future));
            it = m_futures.erase(it);
        }
        else {
            ++it;
        }
    }

    for (auto& jobFinishedPair : finishedJobs) {
        UpdateJob job = jobFinishedPair.second.get();
        if (job.isSuccessful) {
            if (job.name == FRAME_STATE_TERRAIN) {
                m_frameStateService->SwapBuffer(FRAME_STATE_PIPELINE);
                continue;
            }
            m_frameStateService->SwapBuffer(job.name);
        }
    }

}


void UpdateManager::ResetState(std::string stateName) {
}
void UpdateManager::UpdateState(std::string stateName) {
    //auto func = GetUpdateFunctionByStateName(stateName, 0.2f);

}

// Operaciones de gestión de los buffers
void UpdateManager::SwapBuffers() {
    m_frameStateService->SwapBuffers();
}

void UpdateManager::Lock() {
    m_frameStateService->LockAll();
}

void UpdateManager::Unlock() {
    m_frameStateService->UnlockAll();
}

void UpdateManager::Shutdown()
{
    m_mesh = nullptr;
    m_lighting = nullptr;
    m_cameraManager = nullptr;
    m_world = nullptr;
    m_water = nullptr;
    m_frameStateService = nullptr;
    // Intentionally do not toggle ThreadedService::m_running here; managers
    // are controlled via Engine context isRunning flag.
}
