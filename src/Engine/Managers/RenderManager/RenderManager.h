#pragma once

#include <string>
#include <vector>
#include <memory>
#include <IManager.h>
#include <ILifeCycle.h>
#include <Config/Services/ServiceConfig.h>
#include "BaseRenderManager.h"
#include "GameRenderManager.h"

class RenderManager : public IManager, public ILifeCycle {
public:
	RenderManager();
	~RenderManager() override;
	HRESULT Init() override;
	void Shutdown() override;
	void ExecRender();
	void BeginRender();
	void Render() override;
	void EndRender();
	void Update(float deltaTime) override;

	const std::string& GetManagerName() const override {
		static const std::string name = "RenderManager";
		return name;
	}
	static const std::string& GetStaticManagerName()
	{
		static const std::string name = "RenderManager";
		return name;
	}
private:
	BaseRenderManager* m_baseRenderManager;
	GameRenderManager* m_gameRenderManager;
	std::shared_ptr<ServiceConfig> m_serviceConfig;
};
