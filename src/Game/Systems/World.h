#pragma once
#include <windows.h>
#include <string>
#include <memory>
#include <chrono>
#include <IService.h>
#include <KeyboardManager.h>
#include <CameraManager.h>
#include <Config/Game/System/WorldConfig.h>
#include <Defines/WorldDefinition.h>
#include <UI/UIText.h>
#include <Game/Systems/Terrain.h>
#include <Game/Systems/Chronos.h>

class Mouse;
class Line;
class MeshAsset;

class World : public IService
{
public:
	World();
	~World() override;

	HRESULT Init() override;
	HRESULT InitManagers();
	HRESULT PostInit();
	HRESULT InitConfig();
	HRESULT InitSystems();
	void Render() override;
	void Update(float deltaTime) override;
	void Shutdown() override;
	const std::string& GetServiceName() const override {
		static const std::string name = "World";
		return name;
	}
	static const std::string& GetStaticServiceName()
	{
		static const std::string name = "World";
		return name;
	}

	bool HasHeight();

	std::shared_ptr<Terrain> GetTerrain() const { return m_terrain; }

	const std::chrono::system_clock::time_point& GetCurrentDateTime();
	const std::chrono::system_clock::time_point& GetCurrentSimulatedDateTime();
	const float GetCurrentLatitude() const { return m_latitude; }

private:
	Line* m_line;
	WorldDefinition::Gameflags m_gameFlags;
	std::shared_ptr<CameraManager> m_cameraManager;
	std::shared_ptr<WorldConfig> m_config;
	std::shared_ptr<Chronos> m_chronos;
	std::shared_ptr<Terrain> m_terrain;
	std::unique_ptr<UIText> m_text;
	std::unique_ptr<UIText> m_text2;
	std::shared_ptr<Mouse> m_mouse;
	std::shared_ptr<MeshAsset> m_water;
	int m_timeSpeed;
	float m_latitude;	
};
