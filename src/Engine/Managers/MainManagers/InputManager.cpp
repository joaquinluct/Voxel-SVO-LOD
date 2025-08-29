#include "InputManager.h"
#include "REGISTER_MANAGER_MACRO.h"
#include <ManagerLocator/ManagerLocator.h>

REGISTER_MANAGER_TYPE(InputManager, "InputManager")

//--------------------------------------------------------------------------------------
// Constructor: Se ha movido la inicialización al método Init.
//--------------------------------------------------------------------------------------
InputManager::InputManager() {
}

//--------------------------------------------------------------------------------------
// Destructor: No es necesario un código específico aquí debido a unique_ptr.
//--------------------------------------------------------------------------------------
InputManager::~InputManager() {}

//--------------------------------------------------------------------------------------
// Inicialización: Obtiene los managers a través del ManagerLocator.
//--------------------------------------------------------------------------------------
HRESULT InputManager::Init(HWND* hwnd, int width, int heigth) {
    m_mouseManager = ManagerLocator::GetManager<MouseManager>();
    m_keyboardManager = ManagerLocator::GetManager<KeyboardManager>();

    if (m_mouseManager->Init(hwnd, width, heigth) != S_OK) {
        return E_FAIL;
    }
    if (m_keyboardManager->Init() != S_OK) {
        return E_FAIL;
    }
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Maneja los mensajes de entrada y los delega a los managers.
//--------------------------------------------------------------------------------------
void InputManager::HandleInput(UINT message, WPARAM wParam, LPARAM lParam, float deltaTime) {
    if (m_keyboardManager) {
        m_keyboardManager->HandleMessage(message, wParam, lParam);
    }
    if (m_mouseManager) {
        m_mouseManager->HandleMessage(message, wParam, lParam, deltaTime);
    }
}