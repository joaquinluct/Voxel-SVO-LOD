// MainController.cpp
#include "MainController.h"
#include <ServiceLocator/ServiceLocator.h>
// No es necesario incluir aquí todas las implementaciones de los servicios,
// solo si las usas directamente para inicializarlas aquí.

MainController::MainController(HWND hwnd, int width, int height)
    : m_hwnd(hwnd), m_width(width), m_height(height) {
}

MainController::~MainController() {
    Shutdown(); // Asegurarse de que todo se libere al destruir el controlador
}

HRESULT MainController::Initialize(HWND hwnd, int width, int height) {
    
	// 1. Servicios pre-inicialización
    HRESULT hr = ServiceLocator::InitializeServices(m_serviceConfig->services_pre_init_order);
    if (FAILED(hr)) {
        OutputDebugStringA("MainController Init: Failed to initialize services.\n");
        return hr;
    }
    // 2. Inicializar managers en su orden específico
    hr = ManagerLocator::InitializeManagers(m_config->managers_init_order, hwnd, width, height);
    if (FAILED(hr)) {
        OutputDebugStringA("MainController Init: Failed to initialize managers.\n");
        return hr;
    }
	// 2. Servicios post-inicialización
    hr = ServiceLocator::InitializeServices(m_serviceConfig->services_post_init_order);
    if (FAILED(hr)) {
        OutputDebugStringA("MainController Init: Failed to initialize services.\n");
        return hr;
    }

	m_keyboard = ServiceLocator::GetService<Keyboard>();

    // ... el resto de la inicialización de MainController
    return S_OK;
}

void MainController::Update(float deltaTime) {
	ServiceLocator::UpdateServices(m_serviceConfig->services_update_order, deltaTime);
    ManagerLocator::UpdateManagers(m_config->managers_update_order, deltaTime);
}

void MainController::Render() {
    ServiceLocator::RenderServices(m_serviceConfig->services_render_order);
    ManagerLocator::RenderManagers(m_config->managers_render_order);
}

void MainController::Shutdown() {
    // Liberar subsistemas en orden inverso (o el que sea adecuado para dependencias)
    for (auto it = m_subsystems.rbegin(); it != m_subsystems.rend(); ++it) {
        (*it)->Release();
    }
    m_subsystems.clear(); // Limpiar la lista

    // Limpiar el Service Locator (libera los shared_ptr de los managers)
    ManagerLocator::GetInstance().Shutdown();

    // Los shared_ptr de los managers de MainController se liberarán solos
    // cuando el MainController sea destruido.
}

// Métodos para pasar eventos de ventana
void MainController::HandleKeyboardInput(WPARAM wParam, LPARAM lParam) {
    // Pasa el input al KeyboardManager, no directamente al teclado crudo
    //if (m_keyboardManager) {
    //    m_keyboardManager->ProcessKeyboardInput(wParam, lParam); // Asumiendo un método para procesar raw input
    //}
}

void MainController::HandleMouseInput(WPARAM wParam, LPARAM lParam) {
    //if (m_rawMouse) {
    //    m_rawMouse->Update(m_hwnd); // O un método específico para WM_MOUSEMOVE
    //}
}

void MainController::ToggleFullscreen() {
    // Implementa la lógica de la ventana aquí o en un WindowManager si lo creas como servicio
    // Por ahora, como está en MainWindow, esto es un puente.
    // Idealmente, un WindowManager sería un servicio y se le pediría.
}