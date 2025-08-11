// GameManager.cpp
#include "GameManager.h"
#include <ManagerLocator/ManagerLocator.h>
#include "REGISTER_MANAGER_MACRO.h" // Para auto-registro
#include <windows.h> // Para OutputDebugStringA
#include <DirectXMath.h> // Para XMFLOAT3
#include <Text/Text.h>
#include <filesystem>
#include <ServiceLocator/ServiceLocator.h> // Para obtener otros managers

namespace fs = std::filesystem;

// Auto-registro para ServiceLocator
REGISTER_MANAGER_TYPE(GameManager, "GameManager")

GameManager::GameManager(): m_config(), m_gameConfig() {
    OutputDebugStringA("GameManager constructor called.\n");
}

GameManager::~GameManager() {
    OutputDebugStringA("GameManager destructor called.\n");
}

HRESULT GameManager::InitGameServices() const {
    
    // 1. Inicializar gameServices en su orden específico
    HRESULT hr = ServiceLocator::InitializeServices(m_gameConfig->services_init_order);
    if (FAILED(hr)) {
        OutputDebugStringA("MainController Init: Failed to initialize services.\n");
        return hr;
    }
    return S_OK;
}

HRESULT GameManager::InitGame() {
    m_gameConfig = new GameEngineConfig();
    if (!m_gameConfig || !m_gameConfig->services_init_order.size()) {
        OutputDebugStringA("GameManager: Invalid game configuration.\n");
        return E_FAIL;
	}
    //return InitGameServices();
    return S_OK;
}

HRESULT GameManager::Init() {
   /* OutputDebugStringA("Incializando GameManager...\n");
    OutputDebugStringA("GameManager Init called.\n");
    HRESULT hr = InitGame();
    OutputDebugStringA(("Resultado Init " + std::to_string(hr) + " en GameManager\n").c_str());*/
    HRESULT hr = InitGame();
    return hr;
}

void GameManager::Update(float deltaTime) {
    // Lógica de actualización del juego
    // Utiliza los managers obtenidos:
    // m_cameraManager->Update(deltaTime);
    // m_keyboardManager->ProcessInput();
    ServiceLocator::UpdateServices(m_gameConfig->service_update_order, deltaTime);

}

void GameManager::Render() {
    //ServiceLocator::RenderServices(m_gameConfig->services_render_order);
}

void GameManager::Shutdown() {
    // Lógica de renderizado del juego
    // m_cameraManager->Render(); // O usar la cámara para configurar la vista
}