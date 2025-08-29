#pragma once
#include <windows.h>
#include <string>
#include <memory>
#include <chrono>
#include <IService.h>
#include <KeyboardManager.h>
#include <Config/Game/System/ChronosConfig.h>

class Chronos : public IService
{
public:
	Chronos();
	~Chronos() override;

	HRESULT Init() override;
	HRESULT InitDateAndTime();
	void Render() override;
	void Update(float deltaTime) override;
	void Shutdown() override {};
	const std::string& GetServiceName() const override {
		static const std::string name = "Chronos";
		return name;
	}
	static const std::string& GetStaticServiceName()
	{
		static const std::string name = "Chronos";
		return name;
	}

	void SetTimeSpeed(int speed) { m_timeSpeed = speed; }

	const std::chrono::system_clock::time_point& GetCurrentDateTime() { return m_currentTime; }
	const std::chrono::system_clock::time_point& GetCurrentSimulatedDateTime() { return m_simulatedTime; }

private:
	int m_timeSpeed;
	const std::chrono::system_clock::time_point& m_currentTime;
	std::chrono::system_clock::time_point m_simulatedTime;
};
