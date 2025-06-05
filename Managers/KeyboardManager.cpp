#include "KeyboardManager.h"


KeyboardManager::KeyboardManager(Keyboard* keyboard): m_keyboard(keyboard) {
	if (!m_keyboard) {
		m_keyboard = new Keyboard();
	}	
}


KeyboardManager::~KeyboardManager() {
	// Clean up the keyboard manager
	Release();
}

HRESULT KeyboardManager::Init()
{
	return S_OK;
}

HRESULT KeyboardManager::Render() {	
	return S_OK;
}

HRESULT KeyboardManager::Release()
{
	// Process keyboard input
	return S_OK;
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
