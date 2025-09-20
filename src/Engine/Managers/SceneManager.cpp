#include "SceneManager.h"
#include <Base/Managers/EngineConfig.h>
#include <Base/Managers/RenderManagerConfig.h>
#include <chrono>
#include <Defines/Contants/Flags.h>
#include <Defines/EngineDefinition.h>
#include <Defines/FrameStateDefinition.h>
//#include <Defines/Pipeline.h>
#include <Defines/Types/ThreadTypes.h>
#include <FrameStateForward.h>
#include <Game/GameEngineConfig.h>
#include <Game/Systems/Chronos.h>
#include <Game/Systems/Lighting.h>
#include <Game/Systems/Shadows.h>
#include <Game/Systems/World.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <Locators/Registers/REGISTER_MANAGER_MACRO.h>
#include <Locators/ServiceLocator/ServiceLocator.h>
#include <Managers/CameraManager.h>
#include <Managers/ManagerBase.h>
#include <Managers/RenderManager/Jobs/Update/UpdateCameraJob.h>
#include <Managers/RenderManager/Jobs/Update/UpdateCommonJob.h>
#include <Managers/RenderManager/Jobs/Update/UpdateLightJob.h>
#include <Managers/RenderManager/Jobs/Update/UpdateRenderJob.h>
#include <Managers/RenderManager/Jobs/Update/UpdateTerrainJob.h>
//#include <Managers/RenderManager/Pipeline/PipelineState.h>
//#include <Managers/RenderState/FrameStates/MeshFrameState.h>
#include <Managers/RenderState/FrameStates/PassFrameState.h>
//#include <Managers/RenderState/FrameStates/PipelineFrameState.h>
#include <Managers/RenderState/FrameStates/RenderFrameState.h>
#include <Managers/UpdateManager.h>
#include <memory>
#include <PassConfigBase.h>
#include <Services/FrameStateService.h>
#include <string>
#include <thread>
#include <vector>
#include <Windows.h>
#include <Defines/ConcreteOperations.h>

REGISTER_MANAGER_TYPE(SceneManager, "SceneManager")

SceneManager::SceneManager() :

    m_deviceManager{ nullptr },
    m_updateManager{ nullptr },
    m_cameraManager{ nullptr },
    m_frameStateService{ nullptr },
    u_camera{ nullptr },
    u_lighting{ nullptr },
    u_common{ nullptr },
    u_render{ nullptr },
    u_terrain{ nullptr },
    m_initialPassesCollection{},
    m_passesTypesCollection{},
    m_meshesCollection{}
{

    m_jobContext = std::make_shared<JobContext>();

    m_gameConfig = GameEngineConfig();
    m_renderConfig = RenderManagerConfig();
    m_passConfig = PassConfigBase();
    m_engineConfig = EngineConfig();

    SetFlag(FLAG_HAS_SCENE, false);
}

SceneManager::~SceneManager() {

}

// -----------------------------------------------------------------
// Blucle principal de Actualización de actualización de la escena
// -----------------------------------------------------------------
void SceneManager::RunLoop() {
    while (m_running) {
        Update(m_context->deltaTime);
    }
}

// -----------------------------------------------------------------
// Crea el contexto compartido para las tareas de actualización
// -----------------------------------------------------------------
HRESULT SceneManager::CreateContext() {

    m_jobContext->deviceManager = m_deviceManager;
    m_jobContext->cameraManager = m_cameraManager;
    m_jobContext->chronos = ServiceLocator::GetService<Chronos>();
    m_jobContext->world = ServiceLocator::GetService<World>();
    m_jobContext->ligting = ServiceLocator::GetService<Lighting>();
    m_jobContext->shadows = ServiceLocator::GetService<Shadows>();
    m_jobContext->frameStateService = m_frameStateService;
    m_jobContext->engineContext = Context();

    return S_OK;
}

// -----------------------------------------------------------------
// Inicializa los estados de la escena y crea la escena inicial
// -----------------------------------------------------------------
HRESULT SceneManager::InitStates() {
    JobContext* context = m_jobContext.get();

    CreateScene();

    HRESULT hr = InitOperations();
    if (FAILED(hr)) {
        return hr; // Failed to initialize render operations
    }

    u_camera->Execute(context);
    u_lighting->Execute(context);
    u_common->Execute(context);
    u_render->Execute(context);
    u_terrain->Execute(context);


    SetFlag(FLAG_HAS_SCENE, true);

    CreateJob<UpdateCameraJob>(FRAME_STATE_CAMERA.data(), u_camera);

    return S_OK;
}

// -----------------------------------------------------------------
// Inicializa los pases de renderizado desde la configuración
// -----------------------------------------------------------------
HRESULT SceneManager::InitPasses() {
    /*StringArray passNames = m_renderConfig.passes;
    for (const auto& passName : passNames) {
        std::string configName = passName + "Config";
        std::shared_ptr<PassConfigBase> passConfig = ConfigLocator::GetConfig<PassConfigBase>(configName);
        std::shared_ptr<PipelineState> passState = std::make_shared<PipelineState>();
        passState->MapPassFromConfig(passConfig);
        m_frameStateService->PassState()->AddPass(passState);
        m_initialPassesCollection.push_back(passState);
    }*/
    return S_OK;
}

// -----------------------------------------------------------------
// Inicializa las tareas de actualización necesarias para la escena
// -----------------------------------------------------------------
HRESULT SceneManager::InitUpdateTasks() {
    u_camera = std::make_shared<UpdateCameraJob>();
    u_lighting = std::make_shared<UpdateLightJob>();
    u_common = std::make_shared<UpdateCommonJob>();
    u_render = std::make_shared<UpdateRenderJob>();
    u_terrain = std::make_shared<UpdateTerrainJob>();

    return S_OK;
}

// -----------------------------------------------------------------
// Inicializa los managers necesarios para la escena
// -----------------------------------------------------------------
HRESULT SceneManager::InitManagers() {
    // Inicializa los servicios necesarios
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (!m_deviceManager) {
        return E_FAIL; // Device manager service not available
    }
    m_cameraManager = ManagerLocator::GetManager<CameraManager>();
    if (!m_cameraManager) {
        return E_FAIL; // Camera manager service not available
    }
    m_frameStateService = ServiceLocator::GetService<FrameStateService>();

    return S_OK;
}

HRESULT SceneManager::InitOperations() {
    //RenderFrameState* renderState = m_frameStateService->RenderState(false);

    //PipelineViewPortData vpData = {};
    //vpData.desc = *(m_frameStateService->PipelineState(false)->GetCurrentViewport());
    //renderState->AddInitialOperation(PipelineOperationType::Device_SetViewport, "FirstOperationVP", &vpData);
    //PipelineSetRenderTargetsData srData = {};
    //srData.targetView = m_frameStateService->PipelineState(false)->GetCurrentRenderTargetView();
    //srData.stencilView = m_frameStateService->PipelineState(false)->GetCurrentDepthStencilView();
    //srData.isColorPass = true;
    //renderState->AddInitialOperation(PipelineOperationType::Device_Init_SetRenderTargetView, "FirstOperationRTV", &srData);
    //PipelineRenderTargetViewData rtvData = {};
    //rtvData.data = m_frameStateService->PipelineState(false)->GetCurrentRenderTargetView();
    //rtvData.clearColor = XMFLOAT4{ .4f,.0f,.0f,1.0f };
    //renderState->AddInitialOperation(PipelineOperationType::Device_ClearRenderTargetView, "FirstOperation", &rtvData);
    ////renderState->AddInitialOperation(PipelineOperationType::Mesh_Render_SetInputLayout, "FirstOperationV");    
    //renderState->AddInitialOperation(PipelineOperationType::Device_ClearDepthStencilView, "FirstOperationCDSV");
    //renderState->AddInitialOperation(PipelineOperationType::Device_Init_SetSencilView, "FirstOperationSV");
    //renderState->AddInitialOperation(PipelineOperationType::Device_Init_SetSencilState, "FirstOperationSS");
    //renderState->AddInitialOperation(PipelineOperationType::Device_SetDepthStencilState, "FirstOperationSDSS");
    //renderState->AddInitialOperation(PipelineOperationType::Device_SetRasterizedState, "FirstOperationR");
    //renderState->AddFinalOperation(PipelineOperationType::Device_PresentSwapChain, "LastOperation");

    return S_OK;
}

HRESULT SceneManager::InitServices() {
    // Inicializa el servicio de estados de la escena
    std::vector<std::string> updateOrder = m_serviceConfig.services_update_order;
    for (const auto& serviceName : updateOrder) {
        m_updatableServices.push_back(ServiceLocator::GetService(serviceName));
    }
    updateOrder.clear();
    updateOrder = m_gameConfig.service_update_order;
    for (const auto& gameService : updateOrder) {
        m_updatableGameServices.push_back(ServiceLocator::GetService(gameService));
    }
    updateOrder.clear();
    updateOrder = m_engineConfig.managers_update_order;
    for (const auto& managerName : updateOrder) {
        m_updatableManagers.push_back(ManagerLocator::GetManager(managerName));
    }

    return S_OK;
}

// -----------------------------------------------------------------
// Inicialización general
// -----------------------------------------------------------------
HRESULT SceneManager::Init(EngineContext* context) {
    ManagerBase::Init(context);

    // Inicializa las tareas de actualización
    HRESULT hr = InitUpdateTasks();
    if (FAILED(hr)) {
        return hr; // Failed to initialize update tasks
    }

    // Inicializa los managers necesarios
    hr = InitManagers();
    if (FAILED(hr)) {
        return hr; // Failed to initialize required managers
    }

    hr = InitServices();

    hr = InitPasses();
    if (FAILED(hr)) {
        return hr; // Failed to initialize render passes
    }

    return S_OK;
}

// -----------------------------------------------------------------
// Post-Inicialización
// 
// Preparar la escena por primera vez 
// (Importante: fuera del bucle principal)
// -----------------------------------------------------------------
HRESULT SceneManager::PostInit() {

    // Inicializa los managers necesarios
    m_updateManager = ManagerLocator::GetManager<UpdateManager>();
    if (!m_updateManager) {
        return E_FAIL; // Update manager service not available
    }

    // Crea el contexto de las tareas
    CreateContext();

    // Inicializa los estados de la escena
    InitStates();

    // Inicia con el pintado de la pantalla en las 3 operaciones básicas
    /*m_frameStateService->SwapBuffer(FRAME_STATE_RENDER);
    SetFlag(FLAG_HAS_SCENE, true);*/
    return S_OK;
}

// -----------------------------------------------------------------	
// Crea los pases de renderizado necesarios para la escena actual
// -----------------------------------------------------------------
//void SceneManager::AddPassesToScene(PassFrameState* passState, RenderFrameState* renderState) {
//    while (std::shared_ptr<PipelineState> pass = passState->GetNextPass()) {
//        //renderState->
//    }
//}

void SceneManager::ObtainSceneMeshes() {

}

void SceneManager::CreateScene() {

    /*ClearOperation* oper = {};
    oper->

    m_frameStateService->RenderState(false)->AddOperation(Pipel)*/

}

// ----------------------------------------------------------------
// Crea los trabajos necesarios para actualizar la escena
// Se llama una vez por frame desde el bucle de update principal
// ----------------------------------------------------------------
void SceneManager::Update(float deltaTime) {

    // Actualiza los servicios necesarios
    for (const auto& service : m_updatableServices) {
        if (service) {
            service->Update(deltaTime);
        }
    }
    for (const auto& gameService : m_updatableGameServices) {
        if (gameService) {
            gameService->Update(deltaTime);
        }
    }
    for (const auto& manager : m_updatableManagers) {
        if (manager) {
            manager->Update(deltaTime);
        }
    }

    CreateScene();

    // Pequeña pausa para evitar el uso excesivo de la CPU
    /*std::this_thread::sleep_for(std::chrono::milliseconds(2));
    if (!m_jobContext->world->GetTerrain()->IsGenerating()) {
        CreateJob<UpdateTerrainJob>(FRAME_STATE_TERRAIN.data(), u_terrain);
    }
    CreateJob<UpdateCameraJob>(FRAME_STATE_CAMERA.data(), u_camera);
    CreateJob<UpdateLightJob>(FRAME_STATE_LIGHTING.data(), u_lighting);
    CreateJob<UpdateCommonJob>(FRAME_STATE_COMMON.data(), u_common);*/
    //CreateJob<UpdateRenderJob>(FRAME_STATE_RENDER.data(), u_render);
}

// ----------------------------------------------------------------
// Libera los recursos y detiene el bucle de actualizacións
// ----------------------------------------------------------------
void SceneManager::Shutdown() {
    m_running = false;
    m_frameStateService->Shutdown();
    ManagerBase::Shutdown();
}
