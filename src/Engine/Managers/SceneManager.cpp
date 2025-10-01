#include "SceneManager.h"
#include <Assets/Base/MeshAssetBase.h>
#include <Assets/Base/ShaderAsset.h>
#include <Base/Managers/EngineConfig.h>
#include <Base/Managers/RenderManagerConfig.h>
#include <cstdlib>
#include <d3d11.h>
#include <Defines/Contants/Flags.h>
#include <Defines/Contants/Flags/ShaderResources.h>
#include <Defines/Contants/Flags/World.h>
#include <Defines/Contants/FrameState.h>
#include <Defines/EngineDefinition.h>
#include <Defines/Structs/PipelineResources.h>
#include <Defines/Types/ThreadTypes.h>
#include <dxgiformat.h>
#include <Game/GameEngineConfig.h>
#include <Game/Systems/Chronos.h>
#include <Game/Systems/Lighting.h>
#include <Game/Systems/Shadows.h>
#include <Game/Systems/Water.h>
#include <Game/Systems/World.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <Locators/Registers/REGISTER_MANAGER_MACRO.h>
#include <Locators/ServiceLocator/ServiceLocator.h>
#include <Managers/CameraManager.h>
#include <Managers/ManagerBase.h>
#include <Managers/PipelineResourcesManager.h>
#include <Managers/RenderManager/Jobs/Update/UpdateCameraJob.h>
#include <Managers/RenderManager/Jobs/Update/UpdateConstantBuffersJob.h>
#include <Managers/RenderManager/Jobs/Update/UpdateLightJob.h>
#include <Managers/RenderManager/Jobs/Update/UpdateMeshJob.h>
#include <Managers/RenderManager/Jobs/Update/UpdateMeshTerrainJob.h>
#include <Managers/RenderManager/Jobs/Update/UpdateRenderJob.h>
#include <Managers/RenderManager/Jobs/Update/UpdateTerrainJob.h>
#include <Managers/RenderManager/Pipeline/ConcreteOperations.h>
#include <Managers/RenderState/FrameStates/ConstantsBufferFrameState.h>
#include <Managers/RenderState/FrameStates/RenderFrameState.h>
#include <Managers/UpdateManager.h>
#include <map>
#include <memory>
#include <PassConfigBase.h>
#include <Services/FrameStateService.h>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <Windows.h>
#include <wrl/client.h>


REGISTER_MANAGER_TYPE(SceneManager, "SceneManager")

SceneManager::SceneManager() :

    m_deviceManager{ nullptr },
    m_updateManager{ nullptr },
    m_cameraManager{ nullptr },
    m_frameStateService{ nullptr },
    u_camera{ nullptr },
    u_lighting{ nullptr },
    u_constantBuffer{ nullptr },
    u_render{ nullptr },
    u_terrain{ nullptr },
    u_meshTerrain{ nullptr },
    u_mesh{ nullptr }
    //m_initialPassesCollection{},
    //m_passesTypesCollection{},
    //m_meshesCollection{}
{

    m_jobContext = std::make_shared<JobContext>();

    m_gameConfig = GameEngineConfig();
    m_renderConfig = RenderManagerConfig();
    m_passConfig = PassConfigBase();
    m_engineConfig = EngineConfig();

    SetFlag(FLAG_HAS_SCENE, false);
}

SceneManager::~SceneManager() {}

// -----------------------------------------------------------------
// Blucle principal de Actualización de actualización de la escena
// -----------------------------------------------------------------
const float MAX_DELTA_TIME = 0.1f;
const float MIN_DELTA_TIME = 0.0042f;
void SceneManager::RunLoop() {
    while (m_running) {
        float deltaTime = m_context->deltaTime;
        if (deltaTime > MAX_DELTA_TIME) {
            deltaTime = MAX_DELTA_TIME;
        }
        else if (deltaTime < MIN_DELTA_TIME) {
            deltaTime = MIN_DELTA_TIME;
        }
        Update(deltaTime);
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
    m_jobContext->water = ServiceLocator::GetService<Water>();
    m_jobContext->ligting = ServiceLocator::GetService<Lighting>();
    m_jobContext->shadows = ServiceLocator::GetService<Shadows>();
    m_jobContext->frameStateService = m_frameStateService;
    m_jobContext->engineContext = Context();
    m_jobContext->resources = m_resources;

    return S_OK;
}

// -----------------------------------------------------------------
// Inicializa los estados de la escena y crea la escena inicial
// -----------------------------------------------------------------
HRESULT SceneManager::InitStates() {
    //JobContext* context = m_jobContext.get();

    //CreateScene();

    //HRESULT hr = InitOperations();
    //if (FAILED(hr)) {
    //    return hr; // Failed to initialize render operations
    //}

    //u_camera->Execute(context);
    //u_lighting->Execute(context);
    //u_common->Execute(context);
    //u_render->Execute(context);
    //u_terrain->Execute(context);


    //SetFlag(FLAG_HAS_SCENE, true);

    //CreateJob<UpdateCameraJob>(FRAME_STATE_CAMERA.data(), u_camera);

    config.hasTerrain = m_jobContext->world->flags.GetFlag(FLAG_WORLD_HAS_TERRAIN);

    return S_OK;
}

// -----------------------------------------------------------------
// Inicializa los pases de renderizado desde la configuración
// -----------------------------------------------------------------
HRESULT SceneManager::InitPasses() {
    /* StringArray passNames = m_renderConfig.passes;
     for (const auto& passName : passNames) {
         std::string configName = passName + "Config";
         std::shared_ptr<PassConfigBase> passConfig = ConfigLocator::GetConfig<PassConfigBase>(configName);
         std::shared_ptr<PipelineState> passState = std::make_shared<PipelineState>();
         passState->MapPassFromConfig(passConfig);
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
    u_constantBuffer = std::make_shared<UpdateConstantBuffersJob>();
    u_render = std::make_shared<UpdateRenderJob>();
    u_terrain = std::make_shared<UpdateTerrainJob>();
    u_meshTerrain = std::make_shared<UpdateMeshTerrainJob>();
    u_mesh = std::make_shared<UpdateMeshJob>();

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
    m_uiManager = ManagerLocator::GetManager<UIManager>();
    if (!m_uiManager) {
        return E_FAIL; // UI manager service not available
    }
    m_resources = ManagerLocator::GetManager<PipelineResourcesManager>();
    if (!m_resources) {
        return E_FAIL; // Resources manager service not available
    }
    //HRESULT hr = m_resources->Init(m_context);
    //if (FAILED(hr)) {
    //    return hr; // Failed to initialize resources manager
    //}

    m_frameStateService = ServiceLocator::GetService<FrameStateService>();
    if (!m_frameStateService) {
        return E_FAIL; // Frame state service not available
    }

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
    /*m_updatableServices.clear();
    m_updatableGameServices.clear();
    m_updatableManagers.clear();*/

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

    config.isInitialized = true;

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

// -----------------------------------------------------------------
// Crea las operaciones principales de la escena
// 
// (clear, setRTV, setDSV, etc...)
// -----------------------------------------------------------------
void SceneManager::CreateMainOperations() {
    //m_resources->UpdatePassesResources();
    PipelineMainInitialResources* mainResources = m_resources->GetInitialResources();

    //if (mainResources == nullptr || !mainResources->isLoaded) return;

    /*std::unique_ptr<SetViewportOperation> setViewOper = std::make_unique<SetViewportOperation>(mainResources->viewport);
    m_frameStateService->RenderState(false)->AddInitialOperation(std::move(setViewOper));*/

    /*std::unique_ptr<SetRenderTargetOperation> setRTVOper = std::make_unique<SetRenderTargetOperation>(mainResources->renderTargetView.Get(), mainResources->depthStencilResource->stencilViewData.Get());
    m_frameStateService->RenderState(false)->AddInitialOperation(std::move(setRTVOper));*/


    // ESTA ES LA ÚNICA OPERACIÓN QUE DEBE ESTAR SIEMPRE
    std::unique_ptr<ClearOperation> clearOper = std::make_unique<ClearOperation>(mainResources->renderTargetView.Get(), mainResources->depthStencilResource->stencilViewData.Get(), mainResources->clearColor);
    m_frameStateService->RenderState(false)->AddInitialOperation(std::move(clearOper));
    // FINAL - ESTA ES LA ÚNICA OPERACIÓN QUE DEBE ESTAR SIEMPRE

    //if (mainResources->isLoaded) return;

    /*D3D11_RASTERIZER_DESC* desc = mainResources->rasterizerDesc;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> state = mainResources->rasterizedState;
    std::unique_ptr<CreateRasterizerStateOperation> rsOper = std::make_unique<CreateRasterizerStateOperation>(m_deviceManager->GetDevice().Get(), *desc, state);
    m_frameStateService->RenderState(false)->AddOperation(std::move(rsOper));*/

    //mainResources->isLoaded = true;
}

// -----------------------------------------------------------------
// Obtiene los mesh que deben renderizarse en un pase concreto
// -----------------------------------------------------------------
std::vector<MeshResource*> SceneManager::GetPassMeshes(RenderPassResource* pass) {
    const auto& meshes = m_resources->GetMeshes();
    std::vector<MeshResource*> result;
    for (const auto& mesh : meshes) {
        if (mesh == nullptr) continue;
        int index = mesh->mesh->GetReadIndex();
        if (mesh->mesh->GetVertexCount(index) <= 0) continue;
        int passes = mesh->mesh->GetRenderPassesValue();
        bool isInPass = pass->id & passes;
        if (!isInPass) continue;
        result.push_back(mesh);
        //std::vector<std::string> passNames = GetRenderPassNames(passes);
    }
    return result;
}

// -----------------------------------------------------------------
// Crea las operaciones iniciales necesarias para un shader
// -----------------------------------------------------------------
void SceneManager::CreateShaderInitialOperations(ShaderResource* shaderResource) {
    //ShaderAsset* currentShader = shaderResource->shader;
    //if (shaderResource->initialOperationsExecuted) return;

    std::vector<PipelineConstantBufferResource> buffers = shaderResource->GetConstantsBuffers();
    std::unique_ptr<BindConstantsBuffersOperation> cbOper = std::make_unique<BindConstantsBuffersOperation>(buffers, 0);
    m_frameStateService->RenderState(false)->AddOperation(std::move(cbOper));

    /*if (m_frameStateService->ConstantBuffersState(false)->HasOperations()) {
        shaderResource->initialOperationsExecuted = true;
    }*/

    /*std::unique_ptr<CreateRasterizerStateOperation> rsOper = std::make_unique<CreateRasterizerStateOperation>(m_deviceManager->GetDevice().Get(), m_resources->GetInitialResources()->rasterizerDesc, m_resources->GetInitialResources()->rasterizedState);
    m_frameStateService->RenderState(false)->AddOperation(std::move(rsOper));*/
}

// -----------------------------------------------------------------
// Crea las operaciones necesarias para renderizar un shader
// -----------------------------------------------------------------
void SceneManager::CreateShaderOperations(std::string_view shaderName) {
    //if (config.shaderName == shaderName) return;

    ShaderResource* shader = m_resources->GetShader(shaderName.data());

    if (!shader || !shader->shader || shader && !shader->flags.GetFlag(FLAG_SHADER_HAS_CONSTANTS_BUFFERS_DEFINED)) {
        shader = m_resources->CreateShaderResource(shaderName.data());
        m_resources->SetShaderResource(shaderName.data(), shader);
    }

    if (shader == nullptr || !shader->shader) return;

    //if (!shader->initialOperationsExecuted) {
    CreateShaderInitialOperations(shader);
    //}

    ShaderAsset* currentShader = shader->shader;
    ID3D11VertexShader* vShader = currentShader->GetVertexShader();
    ID3D11PixelShader* pShader = currentShader->GetPixelShader();
    const std::vector<ID3D11SamplerState*>& samplers = shader->samplers;

    std::unique_ptr<SetSamplerOperation> samplerOper = std::make_unique<SetSamplerOperation>(samplers, 0);
    m_frameStateService->RenderState(false)->AddOperation(std::move(samplerOper));

    std::unique_ptr<SetVertexShaderOperation> vsOper = std::make_unique<SetVertexShaderOperation>(vShader);
    m_frameStateService->RenderState(false)->AddOperation(std::move(vsOper));

    std::unique_ptr<SetPixelShaderOperation> psOper = std::make_unique<SetPixelShaderOperation>(pShader);
    m_frameStateService->RenderState(false)->AddOperation(std::move(psOper));

    std::unique_ptr<SetInputLayoutOperation> ilOper = std::make_unique<SetInputLayoutOperation>(currentShader->GetInputLayout());
    m_frameStateService->RenderState(false)->AddOperation(std::move(ilOper));

    /*ID3D11Buffer* cBuffer = shader->GetConstantBufferByName("MatrixBufferType");
    MatrixDefinition::MatrixBufferType matrixBuffer{};
    matrixBuffer.SetViewMatrix(DirectX::XMMatrixTranspose(m_cameraManager->GetCurrentViewMatrix()));
    matrixBuffer.SetProjectionMatrix(DirectX::XMMatrixTranspose(m_cameraManager->GetCurrentProjectionMatrix()));
    matrixBuffer.SetWorldMatrix(XMMatrixIdentity());
    std::unique_ptr<MapUnmapOperation> mapUnmapOper = std::make_unique<MapUnmapOperation>(cBuffer, &matrixBuffer, matrixBuffer.Size());
    m_frameStateService->RenderState(false)->AddOperation(std::move(mapUnmapOper));

    ID3D11Buffer* camBuffer = shader->GetConstantBufferByName("CameraData");
    CameraMatrix::CameraData cameraBuffer{};
    cameraBuffer.SetCameraPosition(m_cameraManager->GetCurrentCameraPosition());
    std::unique_ptr<MapUnmapOperation> camOper = std::make_unique<MapUnmapOperation>(camBuffer, &cameraBuffer, cameraBuffer.Size());
    m_frameStateService->RenderState(false)->AddOperation(std::move(camOper));*/

    /*std::unique_ptr<UpdateConstantsBufferOperation> mBuffer = std::make_unique<UpdateConstantsBufferOperation>();
    m_frameStateService->RenderState(false)->AddOperation(std::move(mBuffer));*/


    config.shaderName = shaderName.data();
}

// -----------------------------------------------------------------
// Crea las operaciones necesarias para renderizar un mesh
// -----------------------------------------------------------------
void SceneManager::CreateMeshOperations(RenderPassResource* pass, MeshResource* meshResource) {

    MeshAssetBase* mesh = meshResource->mesh;

    if (!pass->hasShader) {
        CreateShaderOperations(mesh->GetShaderAssetName());
    }

    int index = mesh->GetReadIndex();

    ID3D11Buffer* vBuffer = mesh->GetVertexBuffer(index).Get();

    if (vBuffer == nullptr) return;

    ID3D11Buffer* iBuffer = mesh->GetIndexBuffer(index).Get();
    UINT stride = mesh->GetVertexTypeSize();
    UINT indexCount = mesh->GetIndexCount(index);

    if (meshResource->textures.size() > 0) {
        std::unique_ptr<SetTextureOperation> ssOper = std::make_unique<SetTextureOperation>(meshResource->textures, 6);
        m_frameStateService->RenderState(false)->AddOperation(std::move(ssOper));
    }

    std::unique_ptr<SetVertexBufferOperation> vbOper = std::make_unique<SetVertexBufferOperation>(vBuffer, stride, 0);
    m_frameStateService->RenderState(false)->AddOperation(std::move(vbOper));
    std::unique_ptr<SetIndexBufferOperation> ibOper = std::make_unique<SetIndexBufferOperation>(iBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_frameStateService->RenderState(false)->AddOperation(std::move(ibOper));

    std::unique_ptr<SetPrimitiveTopologyOperation > primiOper = std::make_unique<SetPrimitiveTopologyOperation>(mesh->GetPrimitiveTopology());
    m_frameStateService->RenderState(false)->AddOperation(std::move(primiOper));

    std::unique_ptr<DrawIndexedOperation> drawOper = std::make_unique<DrawIndexedOperation>(indexCount, 0, 0);
    m_frameStateService->RenderState(false)->AddOperation(std::move(drawOper));

}

// -----------------------------------------------------------------
// Crea las operaciones necesarias para cada pase de renderizado
// -----------------------------------------------------------------
void SceneManager::CreatePassOperations() {
    std::vector<RenderPassResource*> passes = m_resources->GetPasses();
    for (const auto& pass : passes) {
        if (pass == nullptr || !pass->enabled) continue;

        std::vector<MeshResource*> meshes = GetPassMeshes(pass);
        if (!meshes.size()) continue;

        /*std::unique_ptr<CreateBlendStateOperation> blendOper = std::make_unique<CreateBlendStateOperation>(m_deviceManager->GetDevice().Get(), pass->blendDesc, pass->blendState);*/

        PipelineMainInitialResources* mainResources = m_resources->GetInitialResources();
        std::unique_ptr<SetViewportOperation> setViewOper = std::make_unique<SetViewportOperation>(mainResources->viewport);
        m_frameStateService->RenderState(false)->AddOperation(std::move(setViewOper));

        std::unique_ptr<SetRenderTargetOperation> setRTVOper = std::make_unique<SetRenderTargetOperation>(mainResources->renderTargetView.Get(), mainResources->depthStencilResource->stencilViewData.Get());
        m_frameStateService->RenderState(false)->AddOperation(std::move(setRTVOper));

        D3D11_RASTERIZER_DESC desc = pass->rasterizedDesc;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> state = pass->rasterizerState;
        std::unique_ptr<CreateRasterizerStateOperation> rsOper = std::make_unique<CreateRasterizerStateOperation>(m_deviceManager->GetDevice().Get(), desc, state);
        m_frameStateService->RenderState(false)->AddOperation(std::move(rsOper));

        if (pass->hasShader) {
            CreateShaderOperations(pass->shaderName);
        }

        for (const auto& mesh : meshes) {
            if (mesh == nullptr) continue;
            int index = mesh->mesh->GetReadIndex();
            if (mesh->mesh->GetVertexCount(index) <= 0) continue;
            CreateMeshOperations(pass, mesh);
        }
    }

    // Actualiza los buffers de constantes generales
    m_jobContext->shaders = m_resources->GetShaders();
    CreateJob<UpdateConstantBuffersJob>(FRAME_STATE_CONSTANT_BUFFERS.data(), u_constantBuffer);

    m_jobContext->meshes = m_resources->GetMeshes();
    CreateJob<UpdateMeshJob>(FRAME_STATE_MESHES.data(), u_mesh);
}

// -----------------------------------------------------------------
// Crea la escena actual para ser renderizada
// -----------------------------------------------------------------
void SceneManager::CreateScene() {

    if (isGeneratingScene) return;
    isGeneratingScene = true;

    m_frameStateService->RenderState(false)->Initialize(m_deviceManager);

    // ------------------------------
    // 1. Obtener los mesh que actuan
    // ------------------------------
    // 1.1 Obtener los mesh del mundo
    std::vector<MeshAssetBase*> meshes = m_jobContext->world->GetMeshes();
    // 1.2 Obtener los mesh de la UI
    if (m_uiManager->IsRunnig()) {
        const auto& uiMesh = m_uiManager->GetUIMesh();
        if (uiMesh) {
            meshes.push_back(uiMesh);
        }
    }
    // 1.3 Actualizar los recursos de los mesh
    m_resources->SetMeshes(meshes);

    // --------------------------------------------------------------------
    // 2. Crear las operaciones principales (clear, setRTV, setDSV, etc...)
    // --------------------------------------------------------------------
    //CreateMainOperations();

    // ----------------------------------
    // 2.1 Crear las operaciones por pase
    // ----------------------------------
    if (meshes.size() > 0) {
        CreatePassOperations();
    }
    else {
        bool a = false;
    }

    // --------------------------------------------------
    // 3. Finalizar la lista de operaciones y swap buffer
    // --------------------------------------------------
    bool hasOperations = false;
    /*if (m_frameStateService->RenderState(false)->HasInitialOperations()) {
        m_frameStateService->RenderState(false)->SetInitialCommandList(m_frameStateService->RenderState(false)->FinalizeInitialCommandList());
        hasOperations = true;
    }*/
    if (m_frameStateService->RenderState(false)->HasOperations()) {
        while (m_frameStateService->IsRendering()) {}
        m_frameStateService->RenderState(false)->SetCommandList(m_frameStateService->RenderState(false)->FinalizeCommandList());
        hasOperations = true;
    }

    if (!hasOperations) {
        isGeneratingScene = false;
        return;
    }

    //while (m_frameStateService->IsRendering()) {}

    if (config.isRenderStateInitialized) {
        m_frameStateService->SwapBuffer(FRAME_STATE_RENDER);
    }
    else {
        m_frameStateService->SwapBufferContent(FRAME_STATE_RENDER);
        config.isRenderStateInitialized = true;
    }
    SetFlag(FLAG_HAS_SCENE, true);

    isGeneratingScene = false;
}

// ----------------------------------------------------------------
// Procesa los trabajos de actualización en segundo plano
// Se llama una vez por frame desde el bucle de update principal
// ----------------------------------------------------------------
void SceneManager::ProcessJobs() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_futures.begin(); it != m_futures.end(); ) {
        FutureUpdateJob& future = it->second;
        if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            UpdateJob job = future.get(); // Obtener el resultado de la tarea
            // Aquí puedes manejar el resultado de la tarea si es necesario
            it = m_futures.erase(it); // Eliminar el futuro completado del mapa
        }
        else {
            ++it; // Avanzar al siguiente futuro
        }
    }
}

// ----------------------------------------------------------------
// Crea los trabajos necesarios para actualizar la escena
// Se llama una vez por frame desde el bucle de update principal
// ----------------------------------------------------------------
void SceneManager::Update(float deltaTime) {

    // Actualiza los servicios necesarios
    for (const auto& manager : m_updatableManagers) {
        if (manager) {
            manager->Update(deltaTime);
        }
    }
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

    if (config.hasTerrain) {
        CreateJob<UpdateTerrainJob>(FRAME_STATE_TERRAIN.data(), u_terrain);
        CreateJob<UpdateMeshTerrainJob>(FRAME_STATE_TERRAIN_MESH.data(), u_meshTerrain);
    }

    //ProcessJobs();

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


FutureUpdateJob SceneManager::AddUpdateJob(const std::string& name, std::function<bool()> task, bool allowDuplicates) {
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

// ----------------------------------------------------------------
// Libera los recursos y detiene el bucle de actualizacións
// ----------------------------------------------------------------
void SceneManager::Shutdown() {
    m_running = false;
    m_frameStateService->Shutdown();
    ManagerBase::Shutdown();
}

