#include "KeyboardManager.h"
#include <ManagerLocator/ManagerLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include "REGISTER_MANAGER_MACRO.h"
#include <WinUser.h> // Para las constantes de mensajes de teclado (WM_KEYDOWN, etc.)

//--------------------------------------------------------------------------------------
// Macro de registro del manager
//--------------------------------------------------------------------------------------
REGISTER_MANAGER_TYPE(KeyboardManager, "KeyboardManager");

//--------------------------------------------------------------------------------------
// Constructor y Destructor
//--------------------------------------------------------------------------------------
KeyboardManager::KeyboardManager() : m_keyboard(), m_config(), m_keyBindings() {
}

KeyboardManager::~KeyboardManager() {}

//--------------------------------------------------------------------------------------
// Inicialización de las asociaciones de teclas
//--------------------------------------------------------------------------------------
HRESULT KeyboardManager::InitKeyBindings() {
	m_config = ConfigLocator::GetConfig<KeyboardManagerConfig>();
	if (!m_config) {
		return E_FAIL;
	}

	m_keyBindings = ServiceLocator::GetService<KeyBindings>();
	if (!m_keyBindings) {
		return E_FAIL;
	}

	m_keyBindings->Bind(m_config->key_bindings);
	return S_OK;
}

//--------------------------------------------------------------------------------------
// Inicialización del manager
//--------------------------------------------------------------------------------------
HRESULT KeyboardManager::Init() {
	OutputDebugStringA("Incializando KeyboardManager...\n");
	HRESULT hr = S_OK;
	m_keyboard = ServiceLocator::GetService<Keyboard>();
	if (!m_keyboard) {
		hr = E_FAIL;
		OutputDebugStringA(("Resultado Init " + std::to_string(hr) + " en KeyboardManager\n").c_str());
		return hr;
	}
	hr = InitKeyBindings();
	OutputDebugStringA(("Resultado Init " + std::to_string(hr) + " en KeyboardManager\n").c_str());
	return hr;
}


//--------------------------------------------------------------------------------------
// Apagado del manager
//--------------------------------------------------------------------------------------
void KeyboardManager::Shutdown()
{
}

//--------------------------------------------------------------------------------------
// Métodos para manejar los mensajes de la ventana de Windows
//--------------------------------------------------------------------------------------
void KeyboardManager::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_keyboard) {
		return;
	}

	// Actualiza el estado de la tecla en el servicio Keyboard
	switch (message) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		m_keyboard->SetKey(static_cast<unsigned char>(wParam), true);
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		m_keyboard->SetKey(static_cast<unsigned char>(wParam), false);
		break;
	}
}

//--------------------------------------------------------------------------------------
// Métodos de consulta de estado de teclas
//--------------------------------------------------------------------------------------
bool KeyboardManager::IsKeyDown(int keyCode) const
{
	return m_keyboard->IsKeyDown(keyCode);
}

bool KeyboardManager::IsKeyPressed(int keyCode) const
{
	return m_keyboard->IsKeyPressed(keyCode);
}

bool KeyboardManager::IsKeyReleased(int keyCode) const
{
	return m_keyboard->IsKeyReleased(keyCode);
}

bool KeyboardManager::OnKeyReleased(int keyCode) const
{
	return m_keyboard->IsKeyReleased(keyCode);
}

bool KeyboardManager::IsKeyDown(KeyMoves keyMove) const
{
	return m_keyBindings->IsKeyDown(keyMove);
}

bool KeyboardManager::IsKeyPressed(KeyMoves keyMove) const
{
	return m_keyBindings->IsKeyPressed(keyMove);
}

bool KeyboardManager::IsKeyReleased(KeyMoves keyMove) const
{
	return m_keyBindings->IsKeyReleased(keyMove);
}
