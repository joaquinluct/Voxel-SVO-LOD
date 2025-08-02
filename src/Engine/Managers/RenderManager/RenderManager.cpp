#include <vector>
#include <memory>
#include <variant>
#include "RenderManager.h"
#include <Pipeline/RenderPassLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include "REGISTER_MANAGER_MACRO.h"

REGISTER_MANAGER_TYPE(RenderManager, "RenderManager")

RenderManager::RenderManager() : m_renderOperations{} {
	m_baseRenderManager = new BaseRenderManager();
	m_gameRenderManager = new GameRenderManager();
	m_renderOperations.reserve(100);
}

RenderManager::~RenderManager() {

}

HRESULT RenderManager::InitSubManagers() {
	HRESULT hr = m_baseRenderManager->Init();
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
	m_renderTargetManager = ManagerLocator::GetRenderManager();
	if (!m_renderTargetManager) {
		return E_FAIL;
	}
	hr = m_gameRenderManager->Init();
	if (FAILED(hr)) {
		return hr;
	}
	m_config = ConfigLocator::GetConfig<RenderManagerConfig>();
	if (!m_config) {
		return E_FAIL;
	}
	m_initManager = ManagerLocator::GetManager<InitManager>();
	if (!m_initManager) {
		return E_FAIL;
	}
	return hr;
}

RenderPassType GetRenderPassTypeFromString(const std::string& pass) {
	if (pass == "MainColorPass") {
		return RenderPassType::MainColorPass;
	} else if (pass == "ShadowPass") {
		return RenderPassType::ShadowPass;
	} else if (pass == "PostProcessPass") {
		return RenderPassType::PostProcessPass;
	} else if (pass == "Debug") {
		return RenderPassType::Debug;
	}
	return RenderPassType::Unknown;
}

HRESULT RenderManager::InitPasses() {
	if (!m_config) {
		return E_FAIL;
	}
	int index = 0;
	for(std::string pass: m_config->passes) {
		RenderPassType passType = GetRenderPassTypeFromString(pass);
		if (passType == RenderPassType::Unknown) {
			continue; // Skip unknown pass types
		}

		std::shared_ptr<IRenderPass> renderPass = RenderPassLocator::GetRenderPass(pass);
		renderPass->Activate();
		renderPass->Init();
		m_renderPasses[index] = renderPass;
		index++;
	}
	return S_OK;
}

HRESULT RenderManager::Init() {	
	HRESULT hr = InitSubManagers();
	if (FAILED(hr)) {
		OutputDebugStringA("RenderManager: ERROR al inicializar Managers");
		return hr;
	}
	hr = InitPasses();
	if (FAILED(hr)) {
		OutputDebugStringA("RenderManager: ERROR al inicializar los pases");
		return hr;
	}
	return hr;
}

void RenderManager::BeginRender() {	
}

void RenderManager::Render() {
	m_baseRenderManager->Render();
	m_gameRenderManager->Render();
}

void RenderManager::EndRender() {
	/*PipelinePresentSwapChain param = {};
	param.data = m_deviceManager->GetSwapChain();
	AddOperation(PipelineOperationType::Device_PresentSwapChain, param);*/
}

void RenderManager::ExecRender() {

	// CÓDIGO ANTIGUO
	/*BeginRender();
	ServiceLocator::RenderServices(m_serviceConfig->services_render_order);
	Render();
	EndRender();*/

	// CÓDIGO NUEVO
	ClearOperations();
	BeginRender();
	for (const auto& passPair : m_renderPasses) {
		if (passPair.second == nullptr) {
			continue; // Skip null passes
		}

		auto& pass = passPair.second;

		if (pass->IsActive()) {
			std::vector<std::shared_ptr<PipelineOperation>> operations = pass->BeginPass();
			//std::map<std::string, std::shared_ptr<MeshAsset>> meshes = m_gameRenderManager->GetMeshes();
			//for (const auto& meshPair : meshes) {
			//	auto mesh = meshPair.second;
			//	if (mesh) {
			//		std::vector<std::shared_ptr<PipelineOperation>> meshOperations = pass->ExecPass(mesh);
			//		operations.insert_range(operations.end(), meshOperations);
			//	}
			//}
			//// De momento ningún pase devuelve nada en el EndPass
			//pass->EndPass();
			m_renderOperations.insert_range(m_renderOperations.end(), operations);
		}
	}
	EndRender();
	ExecOperations();
}

HRESULT RenderManager::ExecOperations()
{
	std::map<std::string, PipelineData> pipelineStates = m_initManager->GetPipelineStates();

	PipelineStore* pStore = new PipelineStore();
	pStore->SetPipelineStates(pipelineStates);
	std::shared_ptr<PipelineStore> pipelineStore = std::make_shared<PipelineStore>(pStore);

	RenderPipeline::RenderPipelineExecutor* executor = new RenderPipeline::RenderPipelineExecutor(m_context, pipelineStore);

	for (std::shared_ptr<PipelineOperation> oper: m_renderOperations) {
		executor->ExecuteOperation(oper);
	}

	return S_OK;
}

void RenderManager::Update(float deltaTime) {
	m_baseRenderManager->Update(deltaTime);
	m_gameRenderManager->Update(deltaTime);
}

void RenderManager::Shutdown() {
	m_baseRenderManager->Shutdown();
	m_gameRenderManager->Shutdown();
	delete m_baseRenderManager;
	m_baseRenderManager = nullptr;
	delete m_gameRenderManager;
	m_gameRenderManager = nullptr;
}

bool RenderManager::IsRenderColourPassActive() const {
	return m_gameRenderManager->IsRenderColourPassActive();
}
bool RenderManager::IsRenderShadowsPassActive() const {
	return m_gameRenderManager->IsRenderShadowsPassActive();
}

void RenderManager::ClearOperations() {
	if (m_renderOperations.size() > 0) {
		m_renderOperations.clear();
	}
}