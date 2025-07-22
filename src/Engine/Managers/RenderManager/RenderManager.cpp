#include "RenderManager.h"
#include <ServiceLocator/ServiceLocator.h>
#include "REGISTER_MANAGER_MACRO.h"

REGISTER_MANAGER_TYPE(RenderManager, "RenderManager")

RenderManager::RenderManager() {
	m_baseRenderManager = new BaseRenderManager();
	m_gameRenderManager = new GameRenderManager();	
}

RenderManager::~RenderManager() {

}

HRESULT RenderManager::Init() {
	// Initialize game-specific rendering resources here
	HRESULT hr = m_baseRenderManager->Init();
	if (FAILED(hr)) {
		return hr;
	}
	m_serviceConfig = ConfigLocator::GetConfig<ServiceConfig>();
	if (!m_serviceConfig) {
		return E_FAIL;
	}
	hr = m_gameRenderManager->Init();
	return hr;
}

void RenderManager::BeginRender() {
	m_baseRenderManager->BeginRender();
	m_gameRenderManager->BeginRender();
}

void RenderManager::Render() {
	m_baseRenderManager->Render();
	m_gameRenderManager->Render();
}

void RenderManager::EndRender() {
	m_gameRenderManager->EndRender();
	m_baseRenderManager->EndRender();
}

void RenderManager::ExecRender() {
	BeginRender();
	ServiceLocator::RenderServices(m_serviceConfig->services_render_order);
	Render();
	EndRender();
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