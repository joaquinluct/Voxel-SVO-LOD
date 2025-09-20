#include "RenderManager.h"
#include <../Includes/FrameStates.h>
#include <Base/Managers/RenderManagerConfig.h>
#include <Config/Base/Managers/EngineConfig.h>
#include <Config/Base/Pipeline/PipelineConfig.h>
#include <Config/PassConfigBase.h>
#include <Defines/ConcreteOperations.h>
#include <Defines/Contants/Flags.h>
#include <Game/Systems/World.h>
#include <Helpers/PipelineHelper.h>
#include <iostream>
#include <Locators/ConfigLocator/ConfigLocator.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <Locators/ServiceLocator/ServiceLocator.h>
#include <Managers/SceneManager.h>
#include <memory>
#include <mutex>
#include <Pipeline/RenderPassLocator.h>
#include <Services/FrameStateService.h>
#include <vector>

#include "REGISTER_MANAGER_MACRO.h"


REGISTER_MANAGER_TYPE(RenderManager, "RenderManager")

RenderManager::RenderManager() : m_renderOperations{} {
    m_baseRenderManager = new BaseRenderManager();
    m_sceneManager = nullptr;
    m_renderOperations.reserve(100);
    //m_executor = nullptr;
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
// Inicializar el executor del pipeline de render
// ----------------------------------------------------------
void RenderManager::InitPipelineExecutor() {
    /*if (!m_executor) {
        m_executor = new RenderPipeline::RenderPipelineExecutor(m_context, m_deviceManager->GetSwapChain());
    }*/
}

// ----------------------------------------------------------
// Inicializar los managers necesarios para el RenderManager
// ----------------------------------------------------------
HRESULT RenderManager::InitSubManagers()
{
    HRESULT hr = m_baseRenderManager->Init(ManagerBase::m_context);
    if (FAILED(hr)) {
        return hr;
    }
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
        return hr;
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

    /*std::shared_ptr<MeshAsset> terrainMesh = m_sceneManager->RegisterMesh("TerrainMesh");
    m_terrain->SetMesh(terrainMesh);*/

    return hr;
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

    InitPipelineExecutor();

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
    m_frameCount++;

    //OperationMap allOpers = {};

    //RenderFrameState* render = m_frameStateService->RenderState();

    //if (!render) {
    //	return;
    //}

    //allOpers = render->GetAllOperations();
    //if (allOpers.size() == 0) {
    //	return;
    //}

    //if (allOpers.size() < 20) {
    //	bool a = false;
    //}

    //if (m_frameCount < 100) {
    //	int size = static_cast<int>(allOpers.size());
    //	OutputDebugStringA(("Nº OPER: " + ParseInt(size) + "\n").c_str());
    //}

    ////auto lock = m_frameStateService->LockAll();
    ////auto lock = m_frameStateService->LockRenderState();
    //for (auto& oper : allOpers) {
    //	PipelineOperBase* param = oper.second.GetOperationParameter();
    //	m_executor->ExecuteOperation(oper.second.GetOperationType(), m_frameStateService.get(), param);
    //}



    //m_frameStateService->UnlockAll();

    /*OperationMap opers = render->GetInitialOperations();

    for (const auto& oper : opers) {
        AddOperation(oper.second.GetOperationType());
    }

    opers = render->GetMeshOperations();

    for (const auto& oper : opers) {
        AddOperation(oper.second.GetOperationType());
    }

    opers = render->GetFinalOperations();

    for (const auto& oper : opers) {
        AddOperation(oper.second.GetOperationType());
    }*/





}

//void RenderManager::Render() {
//
//	// Obtener los estados actuales del Render
//	FrameStateService* FrameStateService = m_frameStateService.get();	
//
//	// Renderizar servicios y managers que necesiten renderizar algo antes del juego
//	ServiceLocator::RenderServices(m_serviceConfig->services_render_order);
//	ManagerLocator::RenderManagers(m_engineConfig->managers_render_order);
//
//	// Limpiar el buffer de operaciones
//	//ClearOperations();
//
//	// Bloquear el buffer de operaciones
//	//m_frameStateService->Lock();
//
//	// Ejecutar el render base (clear, set rt, etc)
//	BeginRender();
//
//	// Ejecutar el render del juego (pases de render)
//	for (const auto& passPair : m_renderPasses) {
//		if (passPair.second == nullptr || !passPair.second.get()->IsActive()) {
//			continue;
//		}
//
//		// ¡¡Importante!!
//		// Siempre pasar a puntero crudo al pase, ya que los pases se gestionan en un mapa de shared_ptr
//		// Lo mismo con culquier iteración que se haga a partir de ahora y sobretodo dentro del bucle de meshes
//		auto& passShared = passPair.second;
//		auto* pass = passShared.get();
//
//		// 0. Enviar configuración del pase al FrameState
//		FrameStateService->SetRenderPassConfig(pass->GetConfig());
//
//		// 1. Llamar a InitPass
//		BeginPass(pass->GetName());			
//
//		// 2. Obtener los meshes a renderizar en este pase
//		std::map<std::string, std::shared_ptr<MeshAsset>> meshes = pass->GetMeshes(m_sceneManager, FrameStateService);
//
//		// 2.1 Si no hay meshes disponibles para el pase 
//		if (meshes.size() == 0) {
//			continue;
//		}
//
//		// 3. Ejecutar el inicio del pase
//		std::vector<PipelineOperationType> operations = pass->BeginPass(nullptr, FrameStateService);
//			
//		// 4. Iterar por cada mesh  y ejecutar el pase por mesh
//		for (const auto& meshPair : meshes) {
//			// ¡¡Importante!!
//			// Siempre pasar a puntero crudo el pase, ya que los pases se gestionan en un mapa de shared_ptr
//			// ¡¡Sobretodo a partir de ahora y dentro del bucle de meshes!!
//			const auto& meshShared = meshPair.second;
//			const auto mesh = meshShared.get();				
//			if (mesh) {	
//				// 4.1 Actualizar el estado del FrameState con el mesh actual
//				// m_frameStateService->SetMesh(meshShared);
//				// 4.2 Llamar a BeginMesh
//				BeginMesh(mesh);
//				// 4.3 Ejecutar el pase para el mesh actual
//				std::vector<PipelineOperationType> meshOperations = pass->ExecPass(mesh, FrameStateService);
//				// 4.4 Añadir las operaciones del mesh al buffer de operaciones del pase
//				//operations.insert_range(operations.end(), meshOperations);
//			}
//		}
//		// 5. Ejecutar el fin del pase
//		std::vector<PipelineOperationType> endOperations = pass->EndPass();
//
//		// 6. Añadir las operaciones al buffer de operadciones
//		//operations.insert_range(operations.end(), endOperations);
//		//m_renderOperations.insert_range(m_renderOperations.end(), operations);			
//	}
//	// Ejecutar el fin del render (presentar swap chain, etc)
//	EndRender();
//
//	// Ejecutar todas las operaciones acumuladas
//	//ExecOperations();
//
//	// Desbloquear el buffer de operaciones
//	//m_frameStateService->Unlock();
//}

// --------------------------------------------------------------------------
// Update
// --------------------------------------------------------------------------
void RenderManager::Update(float deltaTime) {
    m_baseRenderManager->Update(deltaTime);
    m_sceneManager->Update(deltaTime);
}

// --------------------------------------------------------------------------
// Shutdown
// --------------------------------------------------------------------------
void RenderManager::Shutdown() {
    m_baseRenderManager->Shutdown();
    m_sceneManager->Shutdown();
    delete m_baseRenderManager;
    m_baseRenderManager = nullptr;
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
