#pragma once

#include <../Includes/FrameStateForward.h>
#include <Config/Base/Managers/EngineConfig.h>
#include <Config/Base/Managers/RenderManagerConfig.h>
#include <Config/Game/GameEngineConfig.h>
#include <Config/PassConfigBase.h>
#include <Config/Services/ServiceConfig.h>
#include <Defines/Enums/Pass.h>
#include <ManagerBase.h>
#include <memory>
#include <RenderManager/Jobs/Update/UpdateCameraJob.h>
#include <string>
#include <UpdateManager.h>
#include <vector>
#include <Windows.h>

class IService;
class CameraManager;
class DeviceManager;
//class UpdateCameraJob;
class UpdateLightJob;
class UpdateCommonJob;
class UpdateRenderJob;
class UpdateTerrainJob;
struct JobContext;
class FrameStateService;
class PassFrameState;
class PipelineState;
class MeshAsset;

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

    // -----------------
    // Crear los pases
    // -----------------
    //void AddPassesToScene(PassFrameState* passState, RenderFrameState* frameState);

    // ----------------------
    // Contexto para los Jobs
    // ----------------------
    HRESULT CreateContext();

private:
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
    std::shared_ptr<UpdateCommonJob> u_common;
    std::shared_ptr<UpdateTerrainJob> u_terrain;

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
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<UpdateManager> m_updateManager;
    std::shared_ptr<JobContext> m_jobContext;
    std::shared_ptr<FrameStateService> m_frameStateService;

    // ---------------------------------------
    // Estado de la escena
    // ---------------------------------------
    std::vector<std::shared_ptr<PipelineState>> m_initialPassesCollection;
    std::vector<RenderPassType> m_passesTypesCollection;
    std::vector<std::shared_ptr<MeshAsset>> m_meshesCollection;
    std::vector<std::shared_ptr<IService>> m_updatableServices;
    std::vector<std::shared_ptr<ManagerBase>> m_updatableManagers;
    std::vector<std::shared_ptr<IService>> m_updatableGameServices;
};
