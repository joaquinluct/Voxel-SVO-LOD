#pragma once
#define NOMINMAX 
#include <ManagerLocator/ManagerLocator.h>
#include <windows.h>
#include <IService.h>
#include <DeviceManager.h>

//-----------------------------------------------------------------------------
// Clase Mouse - Servicio de entrada de ratón
//-----------------------------------------------------------------------------
class Mouse : public IService
{
public:
    Mouse();
    ~Mouse() override;

    HRESULT Init() override;
    void Shutdown() override;
    void Render() override;
    void Update(float deltaTime) override;

    const std::string& GetServiceName() const override { static const std::string name = "Mouse"; return name; }
    static const std::string& GetStaticServiceName() { static const std::string name = "Mouse"; return name; }

    // Métodos públicos para obtener información del ratón
    int GetDeltaX() const { return m_deltaX; }
    int GetDeltaY() const { return m_deltaY; }
    
    // Métodos de control del cursor
    void SetCenter();
    void ConfineCursor();
    void ShowCursor(bool show);

    void SetLeftButtonState(bool state) { m_isLeftButtonDown = state; m_wasLeftButtonDown = !state; }
    void SetRightButtonState(bool state) { m_isRightButtonDown = state; m_wasRightButtonDown = !state; }
    void SetMiddleButtonState(bool state) { m_isMiddleButtonDown = state; m_wasMiddleButtonDown = !state; }
	bool IsLeftButtonDown() const { return m_isLeftButtonDown; }
	bool IsRightButtonDown() const { return m_isRightButtonDown; }
	bool IsMiddleButtonDown() const { return m_isMiddleButtonDown; }
	bool WasLeftButtonDown() const { return m_wasLeftButtonDown; }
	bool WasRightButtonDown() const { return m_wasRightButtonDown; }
	bool WasMiddleButtonDown() const { return m_wasMiddleButtonDown; }
	void ResetButtonStates() { m_wasLeftButtonDown = m_isLeftButtonDown; m_wasRightButtonDown = m_isRightButtonDown; m_wasMiddleButtonDown = m_isMiddleButtonDown; }

private:
    //std::shared_ptr<RenderTargetManager> m_renderTargetManager;
    std::shared_ptr<DeviceManager> m_deviceManager;
    const HWND* m_hwnd;
    float m_width, m_height;
    float m_lastX, m_lastY;
    int m_deltaX, m_deltaY;
    bool m_firstMouse;
    bool m_ignoreNextDelta = false;
	bool m_isLeftButtonDown = false;
	bool m_isRightButtonDown = false;
	bool m_isMiddleButtonDown = false;
	bool m_wasLeftButtonDown = false;
	bool m_wasRightButtonDown = false;
	bool m_wasMiddleButtonDown = false;
};