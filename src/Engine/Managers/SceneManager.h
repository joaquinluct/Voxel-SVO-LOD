#pragma once

#include <Assets/Base/MeshAssetBase.h>
#include <Config/Base/Managers/EngineConfig.h>
#include <Config/Base/Managers/RenderManagerConfig.h>
#include <Config/Game/GameEngineConfig.h>
#include <Config/PassConfigBase.h>
#include <Config/Services/ServiceConfig.h>
#include <Defines/Enums/Pass.h>
#include <Defines/Structs/PipelineResources.h>
#include <Defines/Types/ThreadTypes.h>
#include <ManagerBase.h>
#include <Managers/PipelineResourcesManager.h>
#include <Managers/UIManager.h>
#include <Managers/UpdateManager.h>
#include <Engine/Rendering/RenderCommand.h>
#include <map>
#include <memory>
#include <mutex>
#include <RenderManager/Jobs/Update/UpdateCameraJob.h>
#include <RenderManager/Jobs/Update/UpdateConstantBuffersJob.h>
#include <Services/ThreadPool.h>
#include <string>
#include <string_view>
#include <vector>
#include <Windows.h>


class IService;
class CameraManager;
class DeviceManager;
//class UpdateCameraJob;
class UpdateLightJob;
class UpdateCommonJob;
class UpdateMeshJob;
class UpdateRenderJob;
class UpdateTerrainJob;
class UpdateMeshTerrainJob;
struct JobContext;
class FrameStateService;
class PassFrameState;
class PipelineState;
class MeshAsset;
class SceneSystem;

class SceneManager : public ManagerBase {
public:
    SceneManager();
    ~SceneManager() override;

    // -------------------
    // Interfaz ILifeCycle
    // -------------------
    HRESULT Init(EngineContext* context) override;
    HRESULT PostInit() override;
    void Shutdown() override;
    void Render() override {};
    void Update(float deltaTime) override;

    const std::string& GetManagerName() const override { static const std::string name = "SceneManager"; return name; }
    static const std::string& GetStaticManagerName() { static const std::string name = "SceneManager"; return name; }

    // ------------------------
    // Interfaz ThreadedService
    // ------------------------
    void RunLoop() override;
    // Llenar el CommandBuffer con comandos de render para el frame actual.
    void FillCommandBuffer(CommandBuffer& buffer);

    // -----------------
    // Inicialización
    // -----------------
    HRESULT InitUpdateTasks();
    HRESULT InitManagers();
    HRESULT InitStates();
    HRESULT InitPasses();
    HRESULT InitOperations();
    HRESULT InitServices();

    // -----------------
    // Crear la escena
    // -----------------
    void CreateScene();
    void ObtainSceneMeshes();
    void CreateMainOperations();
    void CreatePassOperations();
    void CreateShaderOperations(std::string_view shaderName);
    void CreateShaderInitialOperations(ShaderResource* shaderResource);
    void CreateMeshOperations(RenderPassResource* pass, MeshResource* mesh);

    std::vector<MeshResource*> GetPassMeshes(RenderPassResource* pass);

    // -----------------
    // Crear los pases
    // -----------------
    //void AddPassesToScene(PassFrameState* passState, RenderFrameState* frameState);

    // ----------------------
    // Contexto para los Jobs
    // ----------------------
    HRESULT CreateContext();

    FutureUpdateJob AddUpdateJob(const std::string& name, std::function<bool()> task, bool allowDuplicates);
    void ProcessJobs();

    // Migration accessor: obtain SceneSystem facade if created during PostInit
    class SceneSystem* GetSceneSystem() const { return m_sceneSystem ? m_sceneSystem.get() : nullptr; }

private:
    friend class SceneSystem;
    std::unique_ptr<class SceneSystem> m_sceneSystem = nullptr;

    void FillCommandBufferImpl(CommandBuffer& buffer);
    void UpdateImpl(float deltaTime);

    // ---------------------------------------
    // Plantilla de creación de tareas
    // ---------------------------------------
    template<typename T>
    void CreateJob(std::string jobName, std::shared_ptr<T> jobPtr) {
        m_updateManager->AddUpdateJob(
            jobName,
            [context = m_jobContext, job = jobPtr.get(), jobName = jobName]() -> bool {
                return job->Execute(context.get());

                /*UpdateJob result;
                result.name = jobName;
                result.startTime = std::chrono::high_resolution_clock::now();
                result.cancel_token = std::make_shared<std::atomic<bool>>(false);
                return result;*/
            },
            false
        );
    }

    // ---------------------------------------
    // Tareas de actualización del motor
    // ---------------------------------------
    std::shared_ptr<UpdateCameraJob> u_camera;
    std::shared_ptr<UpdateLightJob> u_lighting;
    std::shared_ptr<UpdateRenderJob> u_render;
    std::shared_ptr<UpdateConstantBuffersJob> u_constantBuffer;
    std::shared_ptr<UpdateTerrainJob> u_terrain;
    std::shared_ptr<UpdateMeshTerrainJob> u_meshTerrain;
    std::shared_ptr<UpdateMeshJob> u_mesh;

    // ---------------------------------------
    // Configuración
    // ---------------------------------------
    EngineConfig m_engineConfig{};
    GameEngineConfig m_gameConfig{};
    RenderManagerConfig m_renderConfig{};
    PassConfigBase m_passConfig{};
    ServiceConfig m_serviceConfig{};

    // ---------------------------------------
    // Managers necesarios para las tareas
    // ---------------------------------------
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<UIManager> m_uiManager;
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<UpdateManager> m_updateManager;
    std::shared_ptr<JobContext> m_jobContext;
    std::shared_ptr<FrameStateService> m_frameStateService;
    std::shared_ptr<PipelineResourcesManager> m_resources;

    // ---------------------------------------
    // Estado de la escena
    // ---------------------------------------
    //std::vector<std::shared_ptr<PipelineState>> m_initialPassesCollection;
    //std::vector<RenderPassType> m_passesTypesCollection;
    //std::vector<std::shared_ptr<MeshAsset>> m_meshesCollection;
    std::vector<std::shared_ptr<IService>> m_updatableServices;
    std::vector<std::shared_ptr<ManagerBase>> m_updatableManagers;
    std::vector<std::shared_ptr<IService>> m_updatableGameServices;

    struct Config {
        bool isRenderStateInitialized = false;
        bool isInitialized = false;
        bool hasTerrain = false;
        bool hasPasses = false;
        std::string_view shaderName = "";
    };

    bool isGeneratingScene = false;

    Config config;

    std::mutex m_mutex; // El mutex para proteger la variable compartida

    
};

// (Removed internal friend accessor — use SceneManager::GetSceneSystem())
