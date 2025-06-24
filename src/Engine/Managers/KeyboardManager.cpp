#include "KeyboardManager.h"
#include <ManagerLocator/ManagerLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include "REGISTER_MANAGER_MACRO.h"

REGISTER_MANAGER_TYPE(KeyboardManager, "KeyboardManager");

KeyboardManager::KeyboardManager(): m_keyboard() {
}

KeyboardManager::~KeyboardManager() {}

HRESULT KeyboardManager::Init() {

	m_keyboard = ServiceLocator::GetService<Keyboard>();

	return S_OK;
}

void KeyboardManager::Render() {
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
