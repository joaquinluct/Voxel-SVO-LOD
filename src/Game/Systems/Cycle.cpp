#include "Cycle.h"
#include <ManagerLocator/ManagerLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Util/Utils.h>
#include <Util/DateTime.h>
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(Cycle, "Cycle")

Cycle::Cycle()
	: sun{}, m_lighting{}, m_world{}
{
	
}

Cycle::~Cycle()
{
}

HRESULT Cycle::InitSystems() {
	m_world = ServiceLocator::GetService<World>();
	if (m_world == nullptr) {
		return E_FAIL;
	}
	m_lighting = ServiceLocator::GetService<Lighting>();
	if (m_lighting == nullptr) {
		return E_FAIL;
	}
	Update(0.0f);
	return S_OK;
}

HRESULT Cycle::Init()
{	
	HRESULT hr = InitSystems();
	if (FAILED(hr)) {
		OutputDebugStringA("Cycle: Init date and time fail.\n");
		return hr;
	}
	return hr; // Return success
}

void Cycle::Update(float deltaTime)
{
	float latitude = m_world->GetCurrentLatitude();
	const std::chrono::system_clock::time_point& dateTime = m_world->GetCurrentSimulatedDateTime();
	sun = GetSunData(latitude, dateTime, 2.8f);
	m_lighting->SetLightColor(sun.LightColor);
	m_lighting->SetLightDirection(sun.LightDirection);
}

void Cycle::Render()
{
}
