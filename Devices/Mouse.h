#pragma once

#include <windows.h>
#include "../Interfaces/iRenderizable.h" // Necesario para implementar iRenderizable
#include "../Camera/Camera.h" // Necesario para la clase Camera

//-----------------------------------------------------------------------------
// Clase Mouse
//-----------------------------------------------------------------------------
class Mouse : public iRenderizable
{
public:
    Mouse(Camera* camera, HWND hwnd, int width, int height);
    ~Mouse();

    HRESULT Init() override;
    void Render() override; // No hay nada que renderizar visualmente
    void Release() override;

    void Update(HWND hwnd);
    int GetDeltaX() const { return m_deltaX; }
    int GetDeltaY() const { return m_deltaY; }
    void SetCenter(HWND hwnd, int width, int height);
    void ConfineCursor(HWND hwnd, int width, int height);
    void ShowCursor(bool show);
    float m_pitch; // Cambiar a float
    float m_yaw;   // Cambiar a float

private:
    Camera* m_camera;
    HWND m_hwnd;
    int m_width, m_height;
    float m_x, m_y;
    float m_lastX, m_lastY;
    int m_deltaX, m_deltaY;
    bool m_firstMouse;
    float m_rotationSpeed;
};