#include "Mouse.h"
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(Mouse, "Mouse")

Mouse::Mouse()
    : m_hwnd(nullptr), m_width(), m_height(),
    m_lastX(), m_lastY(),
    m_deltaX(0), m_deltaY(0),
    m_firstMouse(true) {
}

Mouse::~Mouse() {
    Shutdown();
}

HRESULT Mouse::Init() {
    m_deviceManager = ManagerLocator::GetManager<DeviceManager>();
    
    m_hwnd = m_deviceManager->GetHwnd();
    m_width = m_deviceManager->GetWidth();
	m_height = m_deviceManager->GetHeight();

    // Configurar el cursor
    /*ConfineCursor();*/
    /*SetCenter();*/
    ShowCursor(true);
    
    return S_OK;
}

void Mouse::Render() {
    // El servicio de Mouse no necesita renderizar nada
}

void Mouse::Update(float deltaTime) {
    POINT currentScreenPos;
    GetCursorPos(&currentScreenPos); // Posición del cursor en coordenadas de PANTALLA

    m_deltaX = static_cast<int>(currentScreenPos.x - m_lastX);
    m_deltaY = static_cast<int>(currentScreenPos.y - m_lastY);
   
    //SetCenter();
}

void Mouse::SetCenter() {
    POINT center = { static_cast<long>(m_width / 2), static_cast<long>(m_height / 2) };
    ClientToScreen(*m_hwnd, &center);
    SetCursorPos(center.x, center.y);
    
    // Actualizar las coordenadas de referencia para el siguiente frame
    m_lastX = static_cast<float>(center.x);
    m_lastY = static_cast<float>(center.y);
    //m_ignoreNextDelta = true; // Ignorar el delta del siguiente frame
}

void Mouse::ConfineCursor() {
    RECT rect;
    GetClientRect(*m_hwnd, &rect);
    POINT topLeft = { rect.left, rect.top };
    POINT bottomRight = { rect.right, rect.bottom };
    ClientToScreen(*m_hwnd, &topLeft);
    ClientToScreen(*m_hwnd, &bottomRight);
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

void Mouse::Shutdown() {
    // Liberar el cursor de la restricción de la ventana
    ClipCursor(nullptr);
    ShowCursor(true); // Asegurarse de que el cursor se muestre al cerrar
}