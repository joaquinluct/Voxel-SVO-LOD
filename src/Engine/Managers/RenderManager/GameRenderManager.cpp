#include "GameRenderManager.h"
#include <ConfigLocator/ConfigLocator.h>
#include <ServiceLocator/ServiceLocator.h>

GameRenderManager::GameRenderManager() {
	m_gameConfig = new GameEngineConfig();
}

GameRenderManager::~GameRenderManager() {

}

HRESULT GameRenderManager::Init() {
	return S_OK;
}

void GameRenderManager::BeginRender() {

}

void GameRenderManager::Render() {
	ServiceLocator::RenderServices(m_gameConfig->services_render_order);
}

void GameRenderManager::EndRender() {
	// Cleanup or finalize rendering here
}

void GameRenderManager::Update(float deltaTime) {
	// Update game-specific rendering logic here
}

void GameRenderManager::Shutdown() {
	// Release game-specific rendering resources here
}