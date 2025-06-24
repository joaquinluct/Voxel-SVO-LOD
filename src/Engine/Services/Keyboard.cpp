#include "Keyboard.h"
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(Keyboard, "Keyboard")

Keyboard::Keyboard():
    m_keys(),
	m_previousKeys()
{
    for (int i = 0; i < 256; ++i)
    {
        m_keys[i] = 0;
    }
}

Keyboard::~Keyboard()
{
}

HRESULT Keyboard::Init()
{
    // No hay nada específico que inicializar para el teclado
    return S_OK;
}

void Keyboard::Update(float deltaTime)
{
    // Copia el estado actual al estado anterior
    memcpy(m_previousKeys, m_keys, sizeof(m_keys));

    // Obtener el estado actual
    BOOL resul = GetKeyboardState(m_keys);
}

void Keyboard::Render() {}
void Keyboard::Shutdown()
{
    // No hay nada específico que liberar para el teclado
}

bool Keyboard::IsKeyDown(unsigned char key) const
{
    return (m_keys[key] & 0x80) != 0;
}
bool Keyboard::IsKeyUp(unsigned char key) const
{
    return (m_keys[key] & 0x80) == 0;
}
bool Keyboard::IsKeyPressed(unsigned char key) const
{
    return (m_keys[key] & 0x80) != 0 && (m_keys[key] & 0x01) != 0;
}
bool Keyboard::IsKeyReleased(unsigned char key) const
{
    return (m_previousKeys[key] & 0x80) != 0 && (m_keys[key] & 0x80) == 0;
}

bool Keyboard::IsCtrlPressed() const
{
    return (GetKeyState(VK_CONTROL) & 0x8000) != 0;
}
