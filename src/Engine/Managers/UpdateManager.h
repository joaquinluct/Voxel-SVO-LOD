#pragma once

#include <Defines/Usings/ThreadTypes.h>
#include <functional>
#include <ManagerBase.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <windows.h>

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------
class FrameStateBase;
class FrameStateService;
class GigaBufferManager;
class MeshAsset;
struct RenderStateData;
class CameraManager;
class ShaderManager;
class SceneManager;
class Lighting;
class Water;
class World;
class Skybox;
class ThreadPool;

// ---------------------------------------------------------------------------
// UpdateManager: Gestor de actualización del estado de frame
// ---------------------------------------------------------------------------
class UpdateManager : public ManagerBase
{
public:
    UpdateManager();
    ~UpdateManager();

    // Interfaz IManager
    // -----------------
    HRESULT Init(EngineContext* context) override;
    HRESULT PostInit() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    const std::string& GetManagerName() const override { static const std::string name = "UpdateManager"; return name; }
    static const std::string& GetStaticManagerName() { static const std::string name = "UpdateManager"; return name; }

    // Intefaz de ThreadedService
    // -------------------------
    void RunLoop() override;

    // Tareas principales
    // ------------------
    UpdatedJobs UpdateMainData(float deltaTime);
    UpdatedJobs UpdateTerrainData(float deltaTime);

    // Threads
    // -------
    FutureUpdateJob AddUpdateJob(const std::string& name, std::function<bool()> task, bool allowDuplicates);

    void ProcessJobByName(std::string jobName);

    // Operacionesbásicas con los estados de Frame
    // -------------------------------------------
    void ResetState(std::string stateName);
    void UpdateState(std::string stateName);

    // Acceso a los estados del Frame
    //FrameStateService* GetFrameStateService() { return m_frameStateService.get(); }

    /*TerrainFrameState* TerrainState(bool FromReadBuffer = true);
    CameraFrameState* CameraState(bool FromReadBuffer = true);
    LightFrameState* LightState(bool FromReadBuffer = true);
    WaterFrameState* WaterState(bool FromReadBuffer = true);
    CommonFrameState* CommonState(bool FromReadBuffer = true);
    MaterialFrameState* MaterialState(bool FromReadBuffer = true);
    SkyboxFrameState* SkyboxState(bool FromReadBuffer = true);
    TimeFrameState* TimeState(bool FromReadBuffer = true);
    MeshFrameState* MeshState(bool FromReadBuffer = true);
    DeviceFrameState* DeviceState(bool FromReadBuffer = true);
    PassFrameState* PassState(bool FromReadBuffer = true);
    ViewportFrameState* ViewportState(bool FromReadBuffer = true);
    ShaderFrameState* ShaderState(bool FromReadBuffer = true);
    PipelineFrameState* PipelineState(bool FromReadBuffer = true);*/

    // Funciones de actualización de estados
    // -------------------------------------
    FrameStateBase* UpdateTerrainState(float deltaTime);
    FrameStateBase* UpdateCameraState(float deltaTime);
    FrameStateBase* UpdateLightState(float deltaTime);
    FrameStateBase* UpdateWaterState(float deltaTime);
    FrameStateBase* UpdateCommonState(float deltaTime);
    FrameStateBase* UpdateMaterialState(float deltaTime);
    FrameStateBase* UpdateSkyboxState(float deltaTime);
    FrameStateBase* UpdateTimeState(float deltaTime);
    FrameStateBase* UpdateMeshState(float deltaTime);
    FrameStateBase* UpdateDeviceState(float deltaTime);
    FrameStateBase* UpdatePassState(float deltaTime);
    FrameStateBase* UpdateConstantBuffersState(float deltaTime);
    FrameStateBase* UpdateShaderState(float deltaTime);
    FrameStateBase* UpdatePipelineState(float deltaTime);

    // Funciones de reseteo de estados
    // -------------------------------
    //FrameStateBase* ResetMeshState(float deltaTime);
    FrameStateBase* ResetShaderState(float deltaTime);

    //std::function<void()> GetUpdateFunctionByStateName(std::string stateName, float deltaTime);

    // Acceso y gestión del mesh a renderizar
    // --------------------------------------
    const MeshAsset* GetMesh() const { return m_mesh.get(); }
    //void SetMesh(const std::shared_ptr<MeshAsset>& mesh);

    // Métodos para bloqueo y desbloqueo explícito y controlado de buffers
    // -------------------------------------------------------------------
    void SwapBuffers();
    void Lock();
    void Unlock();

    // Ring buffer
    void ExecuteAllUploads();

private:
    // Mesh a renderizar
    // -----------------
    std::shared_ptr<MeshAsset> m_mesh = nullptr;            // Mesh a renderizar

    // Managers
    // --------
    std::shared_ptr<CameraManager> m_cameraManager;         // Manager de las cámaras
    std::shared_ptr<ShaderManager> m_shaderManager;         // Manager de los shaders
    std::shared_ptr<SceneManager> m_sceneManager;           // Manager de la escena
    std::shared_ptr<GigaBufferManager> m_bufferManager;          // Manager de GibaBuffers

    // Game services
    // -------------
    std::shared_ptr<World> m_world;                         // Servicio para gestionar el estado de renderizado    
    std::shared_ptr<Water> m_water;                         // Servicio de agua
    std::shared_ptr<Lighting> m_lighting;                   // Servicio para gestionar cámaras
    std::shared_ptr<Skybox> m_skybox;                       // Servicio de skybox

    std::shared_ptr<FrameStateService> m_frameStateService; // Servicio para gestionar el estado de frame

    // Threading    
    // ---------
    std::shared_ptr<ThreadPool> m_threadPool;               // Servicio de hilos
    //std::vector<FutureUpdateJob> m_futures;
    std::mutex m_mutex; // El mutex para proteger la variable compartida
    std::map<int, FutureUpdateJob> m_futures;
};
