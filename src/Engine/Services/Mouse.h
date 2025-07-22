#pragma once
#define NOMINMAX 
#include <ManagerLocator/ManagerLocator.h>
#include <windows.h>
#include <IService.h>
#include <DeviceManager.h>
#include <RenderTargetManager.h>

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

    const std::string& GetServiceName() const override {
        static const std::string name = "Mouse";
        return name;
    }
    static const std::string& GetStaticServiceName()
    {
        static const std::string name = "Mouse";
        return name;
    }

    // Métodos públicos para obtener información del ratón
    int GetDeltaX() const { return m_deltaX; }
    int GetDeltaY() const { return m_deltaY; }
    
    // Métodos de control del cursor
    void SetCenter();
    void ConfineCursor();
    void ShowCursor(bool show);

private:
    std::shared_ptr<RenderTargetManager> m_renderTargetManager;
    std::shared_ptr<DeviceManager> m_deviceManager;
    const HWND* m_hwnd;
    UINT m_width, m_height;
    float m_lastX, m_lastY;
    int m_deltaX, m_deltaY;
    bool m_firstMouse;
    bool m_ignoreNextDelta = false;
};