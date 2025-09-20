#include "MouseManager.h"
#include <ServiceLocator/ServiceLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <DeviceManager.h>

#include "REGISTER_MANAGER_MACRO.h"

//--------------------------------------------------------------------------------------
// Macro de registro del manager
//--------------------------------------------------------------------------------------
REGISTER_MANAGER_TYPE(MouseManager, "MouseManager")

//--------------------------------------------------------------------------------------
// Constructor y Destructor
//--------------------------------------------------------------------------------------
MouseManager::MouseManager()
{
}

MouseManager::~MouseManager()
{
}

//--------------------------------------------------------------------------------------
// Inicialización
//--------------------------------------------------------------------------------------
HRESULT MouseManager::Init(EngineContext* context)
{
    ManagerBase::Init(context);

    // Obtener el servicio de Mouse a través del ServiceLocator
    m_mouse = ServiceLocator::GetService<Mouse>();
    if (!m_mouse) {
        return E_FAIL; // Error si el servicio no está disponible
    }

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Métodos para manejar los mensajes de la ventana de Windows
//--------------------------------------------------------------------------------------
void MouseManager::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, float deltaTime)
{
    if (message == WM_MOUSEMOVE) {
        m_mouse->Update(deltaTime);
    }
    if (message == WM_LBUTTONDOWN) {
        m_mouse->SetLeftButtonState(true);
	}
    if (message == WM_LBUTTONUP) {
        m_mouse->SetLeftButtonState(false);
    }
    // El MouseManager no necesita procesar los mensajes directamente,
    // ya que la lógica de actualización del mouse se gestiona
    // en su método Update() llamado en el bucle principal.
    // Este método está aquí para mantener la estructura, pero
    // su implementación está vacía.
}
