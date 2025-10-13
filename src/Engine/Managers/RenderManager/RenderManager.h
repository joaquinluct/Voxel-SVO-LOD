#pragma once

#include "BaseRenderManager.h"
#include "Pipeline/ConcreteOperations.h"
#include "RenderPass.h"
#include "SceneManager.h"
#include <Assets/Base/MeshAsset.h>
#include <CameraManager.h>
#include <Config/Base/Managers/EngineConfig.h>
#include <Config/Base/Managers/RenderManagerConfig.h>
#include <Config/PassConfigBase.h>
#include <Config/Services/ServiceConfig.h>
#include <d3d11.h>
#include <Defines/EngineDefinition.h>
#include <DeviceManager.h>
#include <IRenderPass.h>
#include <ManagerBase.h>
#include <map>
#include <memory>
#include <PipelineResourcesManager.h>
#include <RenderManager/Pipeline/PipelineState.h>
#include <Services/FrameStateService.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <wrl/client.h>

// Forward declarations
class World;
class Terrain;

/// <summary>
/// RenderManager
/// 
/// Clase que gestiona el proceso de renderizado en el motor gráfico.
/// </summary>
class RenderManager : public ManagerBase {
public:
    RenderManager();
    ~RenderManager() override;

    // --------------------
    // Interface ILifeCycle
    // --------------------
    HRESULT Init(EngineContext* context) override;
    void Shutdown() override;
    void Render() override;    

    void Update(float deltaTime) override;

    const std::string& GetManagerName() const override { static const std::string name = "RenderManager"; return name; }
    static const std::string& GetStaticManagerName() { static const std::string name = "RenderManager"; return name; }

    // ----------------------------
    // Interface ThreadedService
    // ----------------------------
    void RunLoop() override;

    // --------------------
    // Incialización
    // --------------------
    HRESULT InitSubManagers();
    void InitViewport();
    void InitShader();
    HRESULT InitPasses();
    std::shared_ptr<RenderPass> InitPass(std::string passName);
    HRESULT InitPipelineState();

    // ---------------------------------
    // Funciones de actualización
    // ---------------------------------
    void UpdatePass(std::string passName);

    // --------------------
    // Control del render
    // --------------------
    void BeginRender();
    void BeginPass(std::string passName);
    void BeginMesh(MeshAsset* mesh);
    void EndRender();


    // ----------------------------------------
    // Operaciones del pipeline de renderizado
    // ----------------------------------------
    void AddOperation(PipelineOperationType operationType);
    void ClearOperations();

    // ---------------------------------
    // Getters de servicios y managers
    // ---------------------------------
    //SceneManager* SceneManagerGet() const { return m_sceneManager; };

    // ---------------------------------
    // Control del estado
    // ---------------------------------
    bool IsRenderColourPassActive() const;
    bool IsRenderShadowsPassActive() const;


    void SetPassConfig(PassConfigBase passConfig);

private:
    // Debug
    int m_frameCount = 0;

    // ------------------------------------
    // Servicios y managers espeficíficos
    // ------------------------------------
    std::shared_ptr<PipelineState> m_pipelineState;         // Estado del pipeline de renderizado
    std::shared_ptr<FrameStateService> m_frameStateService; // Servicio para gestionar el estado de frame

    // ------------------------------------
    // Servios y managers generales
    // ------------------------------------
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<PipelineResourcesManager> m_resources;
    std::shared_ptr<World> m_world;
    std::shared_ptr<Terrain> m_terrain;

    // ------------------------------------
    // Configuraciones
    // ------------------------------------
    std::shared_ptr<ServiceConfig> m_serviceConfig;
    std::shared_ptr<EngineConfig> m_engineConfig;
    std::shared_ptr<RenderManagerConfig> m_config;

    std::vector<PipelineOperationType> m_renderOperations = {};
    std::map<int, std::shared_ptr<IRenderPass>> m_renderPasses;
    BaseRenderManager* m_baseRenderManager;
    std::shared_ptr<SceneManager> m_sceneManager;

    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;


    // ------------------------------------
    // Métodos privados
    // ------------------------------------
    //RenderPipeline::RenderPipelineExecutor* GetPipelineExecutor() { return m_executor; }
};
