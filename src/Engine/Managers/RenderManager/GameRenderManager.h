#pragma once

#include <string>
#include <vector>
#include <IManager.h>
#include <ILifeCycle.h>
#include <Config/Game/GameEngineConfig.h>

class GameRenderManager : public IManager, public ILifeCycle {
public:
	GameRenderManager();
	~GameRenderManager() override;
	HRESULT Init() override;
	void Shutdown() override;
	void BeginRender();
	void Render() override;
	void EndRender();
	void Update(float deltaTime) override;

	const std::string& GetManagerName() const override {
		static const std::string name = "GameRenderManager";
		return name;
	}
	static const std::string& GetStaticManagerName()
	{
		static const std::string name = "GameRenderManager";
		return name;
	}
private:
	GameEngineConfig* m_gameConfig{};
};
