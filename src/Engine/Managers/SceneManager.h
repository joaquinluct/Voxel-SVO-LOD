#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <ManagerBase.h>
#include <Config/Game/GameEngineConfig.h>

class DeviceManager;

class SceneManager : public ManagerBase {
public:

	SceneManager();
	~SceneManager() override;
	HRESULT Init() override;
	void Shutdown() override;
	void BeginRender();
	void Render() override;
	void EndRender();
	void Update(float deltaTime) override;
	
	const std::string& GetManagerName() const override {
		static const std::string name = "SceneManager";
		return name;
	}
	static const std::string& GetStaticManagerName()
	{
		static const std::string name = "SceneManager";
		return name;
	}

private:
	GameEngineConfig* m_gameConfig{};
	std::shared_ptr<DeviceManager> m_deviceManager;	
};