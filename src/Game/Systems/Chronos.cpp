#include "Chronos.h"
#include <Util/DateTime.h>
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(Chronos, "Chronos")

Chronos::Chronos()
    : m_currentTime{}, m_simulatedTime{}
{
    //m_currentTime = new std::chrono::system_clock::time_point();
    m_timeSpeed = 200;
}

Chronos::~Chronos()
{
}

HRESULT Chronos::InitDateAndTime()
{
    std::chrono::system_clock::time_point m_currentTime = DateTime::fromStringTimePoint("11/08/2025 10:54:00");
    m_simulatedTime = m_currentTime;
    return 0;
}

HRESULT Chronos::Init()
{
    HRESULT hr = InitDateAndTime();
    if (FAILED(hr)) {
        OutputDebugStringA("Chronos: Init date and time fail.\n");
        return hr;
    }

    return hr; // Return success
}

void Chronos::Update(float deltaTime)
{
    // 1. Obtiene la duración real (deltaTime del motor)
    std::chrono::duration<float> realDeltaDuration(deltaTime);

    // 2. Calcula la duración acelerada
    std::chrono::duration<float> scaledDuration = realDeltaDuration * m_timeSpeed;

    // 3. Suma la duración acelerada a tu tiempo simulado
    m_simulatedTime += std::chrono::duration_cast<std::chrono::system_clock::duration>(scaledDuration);
}

void Chronos::Render()
{
}
