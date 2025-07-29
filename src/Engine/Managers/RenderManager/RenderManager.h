#pragma once

#include <string>
#include <vector>
#include <memory>
#include <IManager.h>
#include <ILifeCycle.h>
#include <DeviceManager.h>
#include <Config/Services/ServiceConfig.h>
#include <Config/Base/RenderManagerConfig.h>
#include "BaseRenderManager.h"
#include "GameRenderManager.h"
#include "InitManager.h"
#include <Managers/RenderTargetManager.h>
#include "RenderPass.h"
#include <IRenderPass.h>
#include <Defines/RenderPass.h>
#include <RenderManager/Pipeline/RenderPipelineExecutor.h>

class RenderManager : public IManager, public ILifeCycle {
public:
	RenderManager();
	~RenderManager() override;
	HRESULT Init() override;
	HRESULT InitSubManagers();
	HRESULT InitPasses();
	void Shutdown() override;
	void ExecRender();
	void Render() override;
	void BeginRender();
	void EndRender();
	void Update(float deltaTime) override;

	HRESULT ExecOperations();

	const std::string& GetManagerName() const override {
		static const std::string name = "RenderManager";
		return name;
	}
	static const std::string& GetStaticManagerName()
	{
		static const std::string name = "RenderManager";
		return name;
	}

	GameRenderManager* GameRenderManagerGet() const { return m_gameRenderManager; };

	bool IsRenderColourPassActive() const;
	bool IsRenderShadowsPassActive() const;

	int AddOperation(const PipelineOperation& operation) {
		m_renderOperations.push_back(std::make_shared<PipelineOperation>(operation));
		return operation.GetPriority();
	}
	int AddOperation(PipelineOperationType operationType, PipelineParameter operationParam = {}) {
		PipelineOperation* operation = new PipelineOperation(operationType, operationParam);
		std::shared_ptr<PipelineOperation> oper = std::make_shared<PipelineOperation>(operation);
		m_renderOperations.push_back(std::make_shared<PipelineOperation>(operation));
		return operation->GetPriority();
	}
	void ClearOperations();

private:
	std::vector<std::shared_ptr<PipelineOperation>> m_renderOperations = {};
	std::map<int, std::shared_ptr<IRenderPass>> m_renderPasses;
	BaseRenderManager* m_baseRenderManager;
	GameRenderManager* m_gameRenderManager;
	std::shared_ptr<DeviceManager> m_deviceManager;
	std::shared_ptr<RenderTargetManager> m_renderTargetManager;
	std::shared_ptr<InitManager> m_initManager;
	std::shared_ptr<ServiceConfig> m_serviceConfig;
	std::shared_ptr<RenderManagerConfig> m_config;	
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
};
