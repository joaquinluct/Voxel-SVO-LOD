#include "BaseRenderManager.h"
#include <ManagerLocator/ManagerLocator.h>

BaseRenderManager::BaseRenderManager() {

}

BaseRenderManager::~BaseRenderManager() {

}

HRESULT BaseRenderManager::Init() {
	m_deviceManager = ManagerLocator::GetDeviceManager();
	if (!m_deviceManager) {
		return E_FAIL; // Device manager not found
	}
	//m_renderTargetManager = ManagerLocator::GetManager<RenderTargetManager>();
	//if (!m_renderTargetManager) {
	//	return E_FAIL; // Render target manager not found
	//}
	return S_OK;
}

void BaseRenderManager::BeginRender() {
	// OJO: COMENTADO ANTES DE LA REFACTORIAZIÓN
	//m_renderTargetManager->Render();
}

void BaseRenderManager::Render() {
	// Render game-specific elements here
}

void BaseRenderManager::EndRender() {
	m_deviceManager->Render();
}

void BaseRenderManager::Update(float deltaTime) {
	// Update game-specific rendering logic here
}

void BaseRenderManager::Shutdown() {
	// Release game-specific rendering resources here
}