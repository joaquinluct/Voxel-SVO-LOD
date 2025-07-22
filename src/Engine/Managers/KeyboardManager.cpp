#include "KeyboardManager.h"
#include <ManagerLocator/ManagerLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include "REGISTER_MANAGER_MACRO.h"

REGISTER_MANAGER_TYPE(KeyboardManager, "KeyboardManager");

KeyboardManager::KeyboardManager(): m_keyboard(), m_config(), m_keyBindings() {
}

KeyboardManager::~KeyboardManager() {}

HRESULT KeyboardManager::InitKeyBindings() {
	m_config = ConfigLocator::GetConfig<KeyboardManagerConfig>();
	if (!m_config) {
		return E_FAIL; // Configuration not available
	}

	m_keyBindings = ServiceLocator::GetService<KeyBindings>();
	if (!m_keyBindings) {
		return E_FAIL; // Key bindings service not available
	}

	m_keyBindings->Bind(m_config->key_bindings);
	// Initialize key bindings from configuration
	//m_config->
	// Register key bindings with the keyboard service
	/*for (const auto& binding : m_config->key_bindings) {
		m_keyboard->RegisterKeyBinding(binding.first, binding.second);
	}*/
	return S_OK;
}

HRESULT KeyboardManager::Init() {
    OutputDebugStringA("Incializando KeyboardManager...\n");
    HRESULT hr = S_OK;
	m_keyboard = ServiceLocator::GetService<Keyboard>();
	if (!m_keyboard) {
		hr = E_FAIL; // Keyboard service not available
		OutputDebugStringA(("Resultado Init " + std::to_string(hr) + " en KeyboardManager\n").c_str());
		return hr;
	}
	hr = InitKeyBindings();
    OutputDebugStringA(("Resultado Init " + std::to_string(hr) + " en KeyboardManager\n").c_str());
	return hr;
}


void KeyboardManager::Shutdown()
{
}

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
	return false;
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
