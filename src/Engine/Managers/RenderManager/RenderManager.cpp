#include "REGISTER_MANAGER_MACRO.h"
#include "RenderManager.h"
#include <../Includes/FrameStates.h>
#include <Base/Managers/RenderManagerConfig.h>
#include <chrono>
#include <Config/Base/Managers/EngineConfig.h>
#include <Config/Base/Pipeline/PipelineConfig.h>
#include <Config/PassConfigBase.h>
#include <d3d11.h>
#include <Defines/Contants/Flags.h>
#include <Game/Systems/World.h>
#include <Helpers/PipelineHelper.h>
#include <iostream>
#include <Locators/ConfigLocator/ConfigLocator.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <Locators/ServiceLocator/ServiceLocator.h>
#include <Managers/RenderManager/Pipeline/ConcreteOperations.h>
#include <Managers/SceneManager.h>
#include <memory>
#include <Pipeline/RenderPassLocator.h>
#include <RenderState/FrameStates/ConstantsBufferFrameState.h>
#include <RenderState/FrameStates/MeshesFrameState.h>
#include <RenderState/FrameStates/RenderFrameState.h>
#include <Services/FrameStateService.h>
#include <Text/Text.h>
#include <vector>
#include <wrl/client.h>

REGISTER_MANAGER_TYPE(RenderManager, "RenderManager")

RenderManager::RenderManager() : m_renderOperations{} {
    m_sceneManager = nullptr;
    m_renderOperations.reserve(100);
    m_pipelineState = nullptr;
    m_deviceManager = nullptr;
    m_context = nullptr;
    m_serviceConfig = nullptr;
    m_frameStateService = nullptr;
    m_cameraManager = nullptr;
    m_engineConfig = nullptr;
    m_config = nullptr;
    m_world = nullptr;
    m_terrain = nullptr;
    m_pipelineState = nullptr;
    m_frameStateService = nullptr;
}

RenderManager::~RenderManager() {

}

// --------------------------------------------------------------------------
// Blucle principal de render
// --------------------------------------------------------------------------
void RenderManager::RunLoop() {
    // Esperar a que la escena tenga datos (con timeout de 15 segundos)
    if (!m_sceneManager->WaitForFlag(FLAG_HAS_SCENE, true, std::chrono::seconds(5))) {
        std::cerr << "[RenderLoop] Timeout esperando a SceneManager. Continuando..." << std::endl;
    }
    // Si no hay estado, no ejecutar el bucle de render
    /*if (!m_sceneManager->GetFlag(FLAG_HAS_SCENE)) {
        return;
    }*/
    // Bucle principal de render
    while (m_running) {
        Render();
    }
}

// ----------------------------------------------------------
// Inicializar los managers necesarios para el RenderManager
// ----------------------------------------------------------
HRESULT RenderManager::InitSubManagers()
{
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (!m_deviceManager) {
        return E_FAIL;
    }
    m_context = m_deviceManager->GetContext();
    m_serviceConfig = ConfigLocator::GetConfig<ServiceConfig>();
    if (!m_serviceConfig) {
        return E_FAIL;
    }
    m_frameStateService = ServiceLocator::GetService<FrameStateService>();
    if (!m_frameStateService) {
        return E_FAIL;
    }
    m_cameraManager = ManagerLocator::GetCameraManager();
    if (!m_cameraManager) {
        return E_FAIL;
    }
    m_sceneManager = ManagerLocator::GetManager<SceneManager>();
    //hr = m_sceneManager->Init(ManagerBase::m_context);
    if (!m_sceneManager) {
        return E_FAIL;
    }
    m_resources = ManagerLocator::GetManager<PipelineResourcesManager>();
    if (!m_resources) {
        return E_FAIL; // Resources manager service not available
    }
    m_engineConfig = ConfigLocator::GetConfig<EngineConfig>();
    if (!m_engineConfig) {
        return E_FAIL;
    }
    m_config = ConfigLocator::GetConfig<RenderManagerConfig>();
    if (!m_config) {
        return E_FAIL;
    }
    m_world = ServiceLocator::GetService<World>();
    if (!m_world) {
        return E_FAIL;
    }
    m_terrain = m_world->GetTerrain();
    if (!m_terrain) {
        return S_OK;
    }
    m_bufferManager = ManagerLocator::GetManager<GigaBufferManager>();
    if (!m_bufferManager) {
        return E_FAIL;
    }

    /*std::shared_ptr<MeshAsset> terrainMesh = m_sceneManager->RegisterMesh("TerrainMesh");
    m_terrain->SetMesh(terrainMesh);*/

    return S_OK;
}

// ----------------------------------------------------------
// InitPass por nombre
// ----------------------------------------------------------
std::shared_ptr<RenderPass> RenderManager::InitPass(std::string passName) {

    // 1. Actualizar la informaición del pase en el FrameState
    std::string configName = passName + "Config";
    std::shared_ptr<PassConfigBase> pConfig = ConfigLocator::GetConfig<PassConfigBase>(configName);

    PipelinePassStateData passState = {};

    passState.id = pConfig->id;
    passState.enabled = true;
    passState.viewPortState = GetConfigStringValue(pConfig->viewPortState);
    passState.blendState = GetConfigStringValue(pConfig->blendState);
    passState.stencilState = GetConfigStringValue(pConfig->stencilState);
    passState.stencilDef = GetConfigStringValue(pConfig->stencilDef);
    passState.shader = GetConfigStringValue(pConfig->shader_name);
    passState.primitiveTopology = pConfig->primitiveTopology;
    passState.rasterizedState = GetConfigStringValue(pConfig->rasterizedState);
    passState.resolution = pConfig->resolution;
    passState.shaderViewState = GetConfigStringValue(pConfig->shaderViewState);
    passState.shadowTextureView = pConfig->shadowTextureView;
    passState.viewPortState = GetConfigStringValue(pConfig->viewPortState);

    m_pipelineState->SetPassData(passName, passState);

    // 2. Obtener el pase del locator y devolverlo (para iterar y llamar al fichero del pase correspondiente)
    //    Seguramente codigo a extinguir desde que tenemos el PipelineState
    std::shared_ptr<RenderPass> renderPass = RenderPassLocator::GetRenderPass(passName);
    if (!renderPass) {
        return nullptr;
    }
    renderPass->Activate();
    renderPass->Init();
    renderPass->SetName(passName);
    return renderPass;
}

// ----------------------------------------------------------
// UpdatePass
// ----------------------------------------------------------
void RenderManager::UpdatePass(std::string passName) {
    m_pipelineState->SetPassState(passName);

    // Actualizar Constant Buffers del FrameState
    MatrixDefinition::MatrixBufferType matrixType = {};
    matrixType.projectionMatrix = m_frameStateService->CameraState()->GetProjectionMatrix();
    matrixType.viewMatrix = m_frameStateService->CameraState()->GetViewMatrix();
    matrixType.worldMatrix = DirectX::XMMatrixIdentity();
    m_frameStateService->PipelineState()->UpdateConstantBuffer("MatrixBufferType", &matrixType, matrixType.Size());
    CameraMatrix::CameraData cameraData = {};
    cameraData.CameraPosition = m_frameStateService->CameraState()->GetCameraPosition();
    m_frameStateService->PipelineState()->UpdateConstantBuffer("CameraData", &matrixType, matrixType.Size());
}

// ----------------------------------------------------------
// InitPasses
// 
// Inicializar los pases de render definidos en el config del RenderManager
// Se encarga de crear los pases y llamar a su Init(EngineContext* context)
// También prepara los valores por defecto de los pases y los añade al mapa de pases
// para que estén disponibles durante el render
// ----------------------------------------------------------
HRESULT RenderManager::InitPasses() {
    if (!m_config) {
        return E_FAIL;
    }

    // Iteramos por cada pase definido en el config
    int index = 0;
    for (std::string pass : m_config->passes) {
        //RenderPassType passType = GetRenderPassTypeFromString(pass);
        //if (passType == RenderPassType::None) {
        //	continue; // Skip unknown pass types
        //}	

        std::shared_ptr<RenderPass> renderPass = InitPass(pass);
        //renderPass->SetExecutor(m_executor);
        renderPass->SetFrameStateService(m_frameStateService.get());
        //std::shared_ptr<IRenderPass> renderPass = RenderPassLocator::GetRenderPass(pass);
        //renderPass->Activate();
        //renderPass->Init(EngineContext* context);
        //renderPass->SetName(pass);
        m_renderPasses[index] = renderPass;
        index++;
    }
    return S_OK;
}

// ----------------------------------------------------------
// Inicializar el PipelineState
// ----------------------------------------------------------
HRESULT RenderManager::InitPipelineState() {
    std::shared_ptr<PipelineConfig> pipeConfig = ConfigLocator::GetConfig<PipelineConfig>();
    m_pipelineState = std::make_shared<PipelineState>(PipelineState{});
    m_pipelineState->SetMainViewport(pipeConfig->mainViewport);
    m_pipelineState->SetMainStencilView(pipeConfig->mainStencilView);
    m_pipelineState->SetMainStencilState(pipeConfig->mainStencilState);
    m_frameStateService->SetPipelineState(m_pipelineState);
    return S_OK;
}

// --------------------------------------------------------------------------
// INICIALIZACION GENERAL
// --------------------------------------------------------------------------
HRESULT RenderManager::Init(EngineContext* context) {
    ManagerBase::Init(context);

    HRESULT hr = InitSubManagers();
    if (FAILED(hr)) {
        OutputDebugStringA("RenderManager: ERROR al inicializar Managers");
        return hr;
    }

    hr = InitPipelineState();
    //hr = InitPasses();
    if (FAILED(hr)) {
        OutputDebugStringA("RenderManager: ERROR al inicializar los pases");
        return hr;
    }

    return hr;
}

// --------------------------------------------------------------------------
// INICIALIZACIONES DURANTE EL BEGIN_PASS
// --------------------------------------------------------------------------

void RenderManager::InitViewport() {
    AddOperation(PipelineOperationType::Device_SetViewport);
}

void RenderManager::InitShader() {
    //AddOperation(PipelineOperationType::Device_SetShader);
}

// --------------------------------------------------------------------------
// BeginRender
// 
// Ejecutar el render base (clear, set rt, etc)
// --------------------------------------------------------------------------
void RenderManager::BeginRender() {

    /*AddOperation(PipelineOperationType::Device_SetRenderTarget);
    AddOperation(PipelineOperationType::Device_Clear);
    m_frameStateService->PipelineState()->ResetAllConstantBufferUpdateFlag();*/
}

// --------------------------------------------------------------------------
// BeginPass
// --------------------------------------------------------------------------
void RenderManager::BeginPass(std::string passName) {
    if (m_pipelineState->ChangedViewport()) {
        InitViewport();
    }
    if (m_pipelineState->ChangedShader()) {
        InitShader();
    }
    UpdatePass(passName);
}

// --------------------------------------------------------------------------
// BeginMesh
// --------------------------------------------------------------------------
void RenderManager::BeginMesh(MeshAsset* mesh) {
    //const Material* material = mesh->GetMaterial();
    //if (material) {
    //    std::string shaderName = WstringToString(material->GetShaderName());
    //    m_pipelineState->SetShader(shaderName);
    //    std::shared_ptr<ShaderManager> shaderManager = ManagerLocator::GetManager<ShaderManager>();
    //    shaderManager->GetSamplersDescAsVector(material->GetShaderName());
    //    //m_frameStateService->PipelineState()->SetSamplerState()
    //}

    // Actualizar los constant buffers del FrameState
    /*std::vector<std::string> buffers = material->GetConstantBufferNames();
    for (const std::string& bufferName : buffers) {
        m_frameStateService->UpdateConstantBuffer(bufferName);
    }*/
}


// --------------------------------------------------------------------------
// EndPass
// --------------------------------------------------------------------------
void RenderManager::EndRender() {
    //PipelinePresentSwapChain param = {};
    //param.data = m_deviceManager->GetSwapChain();
    //param.data = m_frameStateService->CommonState()->GetSwapChain();
    //AddOperation(PipelineOperationType::Device_PresentSwapChain);
}

// --------------------------------------------------------------------------
// RENDER GENERAL
// --------------------------------------------------------------------------
void RenderManager::Render() {

    ID3D11DeviceContext* context = m_context.Get();
    RenderFrameState* renderState = m_frameStateService->RenderState(true);

    if (!renderState) {
        return;
    }

    // PASO 1:OPERACIONES INCIALES (CLEAR, SET RTV, SET DSV, SET VIEWPORT, etc...)
    ClearOperation* clearOper = new ClearOperation(m_resources->GetInitialResources()->renderTargetView.Get(), m_resources->GetInitialResources()->depthStencilResource->stencilViewData.Get(), m_resources->GetInitialResources()->clearColor);
    clearOper->Execute(context);

    // PASO 2: CONSTANTS BUFFERS
    m_frameStateService->BeginRendering();
    ConstantsBufferFrameState* cbState = m_frameStateService->ConstantBuffersState(true);
    if (cbState && cbState->HasOperations()) {
        cbState->ExecuteMapUnmapOperations(context, cbState->GetConstantBuffers());
        OutputDebugStringA(("Frame ConstantBuffers: " + ParseInt(m_frameCount) + "\n").c_str());
    }
    else {
        bool a = false;
    }
    m_frameStateService->EndRendering();

    m_bufferManager->ExecutePendingCopies(m_context.Get());

    //// Paso 3.1: Actualiar los vértices e índices de los meshes
    //MeshesFrameState* meshState = m_frameStateService->MeshesState(true);
    //if (meshState && meshState->HasOperations()) {
    //    meshState->ExecuteMapUnmapOperations(context);
    //    OutputDebugStringA(("Frame Mesh: " + ParseInt(m_frameCount) + "\n").c_str());
    //}
    //else {
    //    bool a = false;
    //}


    // Paso 3: EJECUTAR LAS OPERACIONES DIFERIDAS
    Microsoft::WRL::ComPtr<ID3D11CommandList> commandList = renderState->GetCommandList();
    if (commandList) {
        m_context->ExecuteCommandList(commandList.Get(), FALSE);
    }
    //else {
        //return;
    //}

    // Paso 4: LANZAR EL PRESENT DEL SWAP CHAIN
    m_deviceManager->GetSwapChain()->Present(1, 0);
    OutputDebugStringA(("Frame SwapChain: " + ParseInt(m_frameCount) + "\n").c_str());


    // DEBUG: Contador de frames
    m_frameCount++;
    if (m_frameCount % 10 == 0) {
        OutputDebugStringA(("Frame: " + ParseInt(m_frameCount) + "\n").c_str());
    }
}

// --------------------------------------------------------------------------
// Update
// --------------------------------------------------------------------------
void RenderManager::Update(float deltaTime) {
    m_sceneManager->Update(deltaTime);
}

// --------------------------------------------------------------------------
// Shutdown
// --------------------------------------------------------------------------
void RenderManager::Shutdown() {
    m_sceneManager->Shutdown();
}

// --------------------------------------------------------------------------
// Control de estado
// --------------------------------------------------------------------------
bool RenderManager::IsRenderColourPassActive() const {
    return true;
    //return m_sceneManager->IsRenderColourPassActive();
}
bool RenderManager::IsRenderShadowsPassActive() const {
    return false;
    //return m_sceneManager->IsRenderShadowsPassActive();
}
void RenderManager::SetPassConfig(PassConfigBase passConfig) {
    m_frameStateService->SetRenderPassConfig(std::make_shared<PassConfigBase>(passConfig));
}

// --------------------------------------------------------------------------
// Métodos de gestión del buffer de operaciones
// --------------------------------------------------------------------------
void RenderManager::AddOperation(PipelineOperationType operationType) {
    //m_executor->ExecuteOperation(operationType, m_frameStateService.get(), {});
}

void RenderManager::ClearOperations() {
    if (m_renderOperations.size() > 0) {
        m_renderOperations.clear();
    }
}
