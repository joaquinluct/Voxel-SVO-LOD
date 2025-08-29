#pragma once

#include <windows.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <future>
#include <chrono>
#include <thread>
#include <ManagerBase.h>
#include <../Includes/FrameStateForward.h>
#include <Defines/Threading.h>
#include <Defines/Types.h>

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------
class FrameStateBase;
class FrameStateService;
class MeshAsset;
struct RenderStateData;
class CameraManager;
class ShaderManager;
class Lighting;
class Water;
class World;
class Skybox;
class ThreadPool;

// Definición de constantes de nombres de las tareas de actualización
using UpdateJob = Threading::Job<StringArray>;
using UpdatedJobs = std::vector<UpdateJob>;
using FutureUpdateJobs = std::future<UpdatedJobs>;

// ---------------------------------------------------------------------------
// UpdateManager: Gestor de actualización del estado de frame
// ---------------------------------------------------------------------------
class UpdateManager : public ManagerBase
{
public:
    UpdateManager();
    ~UpdateManager();

    // Interfaz IManager
    HRESULT Init() override;    
    void Update(float deltaTime) override;
    void Shutdown() override;

    const std::string& GetManagerName() const override { static const std::string name = "UpdateManager"; return name; }
    static const std::string& GetStaticManagerName() { static const std::string name = "UpdateManager"; return name; }

    // Tareas principales
    UpdatedJobs UpdateMainData(float deltaTime);
    UpdatedJobs UpdateTerrainData(float deltaTime);


    // Threads
    void StartThreads(float deltaTime);

    // Operacionesbásicas con los estados de Frame
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
    FrameStateBase* UpdateViewportState(float deltaTime);
    FrameStateBase* UpdateShaderState(float deltaTime);
    FrameStateBase* UpdatePipelineState(float deltaTime);

    FrameStateBase* ResetMeshState(float deltaTime);
    FrameStateBase* ResetShaderState(float deltaTime);

    //std::function<void()> GetUpdateFunctionByStateName(std::string stateName, float deltaTime);

    const MeshAsset* GetMesh() const { return m_mesh.get(); }
    void SetMesh(const std::shared_ptr<MeshAsset>& mesh);

    // Métodos para bloqueo y desbloqueo explícito y controlado de buffers
    void SwapBuffers();
    void Lock();
    void Unlock();

private:
    std::shared_ptr<MeshAsset> m_mesh = nullptr;

    std::shared_ptr<Lighting> m_lighting; // Servicio para gestionar cámaras
    std::shared_ptr<CameraManager> m_cameraManager; // Servicio para gestionar cámaras
    std::shared_ptr<ShaderManager> m_shaderManager; // Servicio para gestionar los shaders
    std::shared_ptr<World> m_world; // Servicio para gestionar el estado de renderizado    
    std::shared_ptr<Water> m_water; // Servicio de agua
    std::shared_ptr<Skybox> m_skybox; // Servicio de skybox
        
    std::shared_ptr<FrameStateService> m_frameStateService; // Servicio para gestionar el estado de frame

	// Threading    
	std::shared_ptr<ThreadPool> m_threadPool; // Servicio de hilos
    std::vector<FutureUpdateJobs> m_futures;
};