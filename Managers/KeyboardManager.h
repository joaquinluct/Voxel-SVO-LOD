#pragma once

#include "../Interfaces/iDirectXManager.h"
#include "../Devices/Keyboard.h"

class KeyboardManager : public iDirectXManager
{
public:
	KeyboardManager(Keyboard* keyboard);
	~KeyboardManager();
	HRESULT Init() override;
	HRESULT Release() override;
	HRESULT Render() override;
	bool IsKeyDown(int keyCode) const;
	bool IsKeyPressed(int keyCode) const;
	bool IsKeyReleased(int keyCode) const;
	bool OnKeyReleased(int keyCode) const;
private:
	Keyboard* m_keyboard;
};
