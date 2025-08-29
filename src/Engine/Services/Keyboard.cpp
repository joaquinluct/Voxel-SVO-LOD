#include "Keyboard.h"
#include <REGISTER_SERVICE_MACRO.h>
#include <WinUser.h>

REGISTER_SERVICE_TYPE(Keyboard, "Keyboard")

Keyboard::Keyboard()
{
    m_keys.fill(false);
    m_previousKeys.fill(false);
}

Keyboard::~Keyboard()
{
}

HRESULT Keyboard::Init()
{
    // No se necesita inicialización especial
    return S_OK;
}

void Keyboard::Shutdown()
{
    // No se necesita liberación especial
}

void Keyboard::Render() {}

// NUEVA LÓGICA: Ahora este método solo se encarga de actualizar el estado anterior
void Keyboard::Update(float deltaTime)
{
    // Bloquea el mutex para copiar el estado actual al anterior
    std::lock_guard<std::mutex> lock(m_mutex);
    m_previousKeys = m_keys;
}

// NUEVO: Método para que WndProc actualice el estado de las teclas
void Keyboard::SetKey(unsigned char key, bool isDown)
{
    // Protege la escritura en el array de teclas con un mutex
    std::lock_guard<std::mutex> lock(m_mutex);
    if (key == VK_UP) {
        bool a = false;
    }
    m_keys[key] = isDown;
}

// Lógica para detectar los estados
bool Keyboard::IsKeyDown(unsigned char key) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_keys[key];
}

bool Keyboard::IsKeyUp(unsigned char key) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return !m_keys[key];
}

bool Keyboard::IsKeyPressed(unsigned char key) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_keys[key] && !m_previousKeys[key];
}

bool Keyboard::IsKeyReleased(unsigned char key) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return !m_keys[key] && m_previousKeys[key];
}

const std::string& Keyboard::GetServiceName() const
{
    static const std::string name = "Keyboard";
    return name;
}

const std::string& Keyboard::GetStaticServiceName()
{
    static const std::string name = "Keyboard";
    return name;
}