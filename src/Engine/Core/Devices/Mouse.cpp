#include "Mouse.h"
#include <cmath> // Para std::fmod

Mouse::Mouse(std::shared_ptr<CameraManager> cameraManager, HWND hwnd, int width, int height)
    : m_camera(), m_hwnd(hwnd), m_width(width), m_height(height),
    m_lastX(static_cast<float>(width / 2)), m_lastY(static_cast<float>(height / 2)),
    m_deltaX(0), m_deltaY(0),
    m_firstMouse(true),
    m_rotationSpeed(0.0005f),
    m_pitch(0.0f), m_yaw(0.0f), m_x(0.0f), m_y(0.0f) {
	m_camera = cameraManager->GetCamera("FistPersonCamera");
}

Mouse::~Mouse() {
    Release();
}

HRESULT Mouse::Init() {
    //ConfineCursor(m_hwnd, m_width, m_height);
    //SetCenter(m_hwnd, m_width, m_height);
    ShowCursor(false);
    return S_OK;
}

void Mouse::Render() {
    Update(m_hwnd);

    if (m_camera) {
        m_pitch += static_cast<float>(m_deltaY * m_rotationSpeed);
        m_yaw += static_cast<float>(m_deltaX * m_rotationSpeed);

        // Limitar pitch
        float pitchLimit = XM_PIDIV2 - 0.01f;
        if (m_pitch > pitchLimit) m_pitch = pitchLimit;
        if (m_pitch < -pitchLimit) m_pitch = -pitchLimit;

        // Mantener yaw dentro de [0, 2PI)
        m_yaw = std::fmod(m_yaw, XM_2PI);
        if (m_yaw < 0) m_yaw += XM_2PI;

        m_camera->SetRotation(m_pitch, m_yaw, 0.0f);
        
        // Centrar el ratón después de la rotación
        //if (m_deltaX < -1500 || m_deltaX > 1500 || m_deltaY < -1500 || m_deltaY > 1500) {
        if (m_deltaX != 0 || m_deltaY != 0) {
            SetCenter(m_hwnd, m_width, m_height);
        }
    }
}

void Mouse::Release() {
    //ShowCursor(true);
    ClipCursor(nullptr);
}

void Mouse::Update(HWND hwnd) {
    POINT currentPos;
    GetCursorPos(&currentPos);
    ScreenToClient(hwnd, &currentPos);

    if (m_firstMouse) {
        m_lastX = static_cast<float>(currentPos.x);
        m_lastY = static_cast<float>(currentPos.y);
        m_firstMouse = false;
        return;
    }

    m_deltaX = static_cast<int>(currentPos.x - m_lastX);
    m_deltaY = static_cast<int>(currentPos.y - m_lastY);

    m_lastX = static_cast<float>(currentPos.x);
    m_lastY = static_cast<float>(currentPos.y);
}

void Mouse::SetCenter(HWND hwnd, int width, int height) {
    POINT center = { width / 2, height / 2 };
    ClientToScreen(hwnd, &center);
    SetCursorPos(center.x, center.y);
    m_lastX = static_cast<float>(center.x);
    m_lastY = static_cast<float>(center.y);
}

void Mouse::ConfineCursor(HWND hwnd, int width, int height) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    POINT topLeft = { rect.left, rect.top };
    POINT bottomRight = { rect.right, rect.bottom };
    ClientToScreen(hwnd, &topLeft);
    ClientToScreen(hwnd, &bottomRight);
    rect.left = topLeft.x;
    rect.top = topLeft.y;
    rect.right = bottomRight.x;
    rect.bottom = bottomRight.y;
    ClipCursor(&rect);
}

void Mouse::ShowCursor(bool show)
{
    if (show)
    {
        ::ShowCursor(TRUE);
    }
    else
    {
        ::ShowCursor(FALSE);
    }
}