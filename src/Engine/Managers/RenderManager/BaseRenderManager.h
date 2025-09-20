#pragma once

#include <string>
#include <vector>
#include <ManagerBase.h>
#include <ILifeCycle.h>
#include "SceneManager.h"
#include <DeviceManager.h>
//#include <RenderTargetManager.h>

class BaseRenderManager : public ManagerBase {
public:
	BaseRenderManager();
	~BaseRenderManager() override;
	HRESULT Init(EngineContext* context) override;
	void Shutdown() override;
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
	std::shared_ptr<DeviceManager> m_deviceManager;
	//std::shared_ptr<RenderTargetManager> m_renderTargetManager;
};
