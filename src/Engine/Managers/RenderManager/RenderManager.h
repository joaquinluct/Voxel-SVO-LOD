#pragma once

#include <string>
#include <vector>
#include <memory>
#include <ManagerBase.h>
#include <ILifeCycle.h>
#include <DeviceManager.h>
#include <CameraManager.h>
#include <Config/Services/ServiceConfig.h>
#include <Config/Base/Managers/RenderManagerConfig.h>
#include <Config/Base/Managers/EngineConfig.h>
#include <Config/PassConfigBase.h>
#include "BaseRenderManager.h"
#include "SceneManager.h"
#include <Services/FrameStateService.h>
#include "RenderPass.h"
#include <IRenderPass.h>
#include <Defines/AreaMesh.h>
#include <RenderManager/Pipeline/RenderPipelineExecutor.h>
#include <RenderManager/Pipeline/PipelineState.h>

class World;
class Terrain;

class RenderManager : public ManagerBase {
public:
	RenderManager();
	~RenderManager() override;
	HRESULT Init() override;
	HRESULT InitSubManagers();
	HRESULT InitPasses();
	std::shared_ptr<RenderPass> InitPass(std::string passName);
	HRESULT InitPipelineState();
	void BeginPass(std::string passName);
	void BeginMesh(MeshAsset* mesh);
	void InitViewport();
	void InitShader();
	void Shutdown() override;
	//void ExecRender();
	void Render() override;
	void BeginRender();
	void EndRender();
	void Update(float deltaTime) override;
	void UpdatePass(std::string passName);

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

	SceneManager* SceneManagerGet() const { return m_SceneManager; };

	bool IsRenderColourPassActive() const;
	bool IsRenderShadowsPassActive() const;

	void AddOperation(PipelineOperationType operationType);
		
	void ClearOperations();

	void SetPassConfig(PassConfigBase passConfig);	

private:	
	// Servicios y managers espeficíficos
	std::shared_ptr<PipelineState> m_pipelineState;         // Estado del pipeline de renderizado
	std::shared_ptr<FrameStateService> m_frameStateService; // Servicio para gestionar el estado de frame
	RenderPipeline::RenderPipelineExecutor* m_executor;     // Ejecutor del pipeline de renderizado

	// Servios y managers generales
	std::shared_ptr<DeviceManager> m_deviceManager;
	std::shared_ptr<CameraManager> m_cameraManager;
	std::shared_ptr<World> m_world;
	std::shared_ptr<Terrain> m_terrain;	

	// Configuraciones
	std::shared_ptr<ServiceConfig> m_serviceConfig;
	std::shared_ptr<EngineConfig> m_engineConfig;
	std::shared_ptr<RenderManagerConfig> m_config;

	std::vector<PipelineOperationType> m_renderOperations = {};
	std::map<int, std::shared_ptr<IRenderPass>> m_renderPasses;
	BaseRenderManager* m_baseRenderManager;
	SceneManager* m_SceneManager;
	
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;


	// Métodos privados
	void InitPipelineExecutor();
	RenderPipeline::RenderPipelineExecutor* GetPipelineExecutor() { return m_executor; }
};
