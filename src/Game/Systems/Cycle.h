#pragma once
#include <windows.h>
#include <string>
#include <memory>
#include <chrono>
#include <IService.h>
#include <KeyboardManager.h>
#include <Defines/Matrix/MatrixDefinitionBase.h>
#include <Config/Game/System/CycleConfig.h>
#include "Cycle/Sun.h"
#include <Game/Systems/World.h>
#include <Game/Systems/Lighting.h>

class Cycle : public IService
{
public:
	Cycle();
	~Cycle() override;

	HRESULT Init() override;
	HRESULT InitSystems();
	void Render() override;
	void Update(float deltaTime) override;
	void Shutdown() override {};
	const std::string& GetServiceName() const override {
		static const std::string name = "Cycle";
		return name;
	}
	static const std::string& GetStaticServiceName()
	{
		static const std::string name = "Cycle";
		return name;
	}

	SunData GetSun() { return sun; }

private:
	SunData sun;
	std::shared_ptr<World> m_world;
	std::shared_ptr<Lighting> m_lighting;
};
