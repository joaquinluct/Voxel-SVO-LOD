#include "Keyboard.h"

Keyboard::Keyboard()
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


void Keyboard::Render()
{
    Update(); // Actualizar el estado del teclado
}
void Keyboard::Release()
{
    // No hay nada específico que liberar para el teclado
}

void Keyboard::Update()
{
    // Copia el estado actual al estado anterior
    memcpy(m_previousKeys, m_keys, sizeof(m_keys));

    // Obtener el estado actual
    BOOL resul = GetKeyboardState(m_keys);
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
