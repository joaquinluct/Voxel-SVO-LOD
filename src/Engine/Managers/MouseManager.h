#pragma once
//#define NOMINMAX
#include <ManagerLocator/ManagerLocator.h>
#include <windows.h>
#include <ManagerBase.h>
#include <Services/Mouse.h>

class DeviceManager;

//-----------------------------------------------------------------------------
// Clase Mouse - Servicio de entrada de ratón
//-----------------------------------------------------------------------------
class MouseManager : public ManagerBase
{
public:
    MouseManager();
    ~MouseManager() override;

    bool IsWindowDependent() const override { return true; }

    HRESULT Init(HWND* hwnd, int width, int heigth) override;

    const std::string& GetManagerName() const override { static const std::string name = "MouseManager"; return name; }
    static const std::string& GetStaticManagerName() { static const std::string name = "MouseManager"; return name; }

    void HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, float deltaTime);
    
    // Métodos públicos para obtener información del ratón
    int GetDeltaX() const { return m_mouse->GetDeltaX(); }
    int GetDeltaY() const { return m_mouse->GetDeltaY(); }

    // Métodos de control del cursor
    void SetCenter() { m_mouse->SetCenter(); }
    void ConfineCursor() { m_mouse->ConfineCursor(); }
    void ShowCursor() { m_mouse->ShowCursor(true); }

	// Métodos para gestionar el estado de los botones del ratón
    void SetLeftButtonState(bool state) { m_mouse->SetLeftButtonState(state); }
    void SetRightButtonState(bool state) { m_mouse->SetRightButtonState(state); }
	void SetMiddleButtonState(bool state) { m_mouse->SetMiddleButtonState(state); }
    bool IsLeftButtonDown() const { return m_mouse->IsLeftButtonDown(); }
    bool IsRightButtonDown() const { return m_mouse->IsRightButtonDown(); }
	bool IsMiddleButtonDown() const { return m_mouse->IsMiddleButtonDown(); }
    bool WasLeftButtonDown() const { return m_mouse->WasLeftButtonDown(); }
	bool WasRightButtonDown() const { return m_mouse->WasRightButtonDown(); }
	bool WasMiddleButtonDown() const { return m_mouse->WasMiddleButtonDown(); }
	void ResetButtonStates() { m_mouse->ResetButtonStates(); }

private:
    std::shared_ptr<DeviceManager> m_deviceManager;
	std::shared_ptr<Mouse> m_mouse;
    const HWND* m_hwnd;    
};