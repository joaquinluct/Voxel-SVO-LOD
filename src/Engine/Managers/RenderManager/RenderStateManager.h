#pragma once

#include <windows.h>
#include <memory>
#include "IManager.h"
#include <ILifeCycle.h>
#include <Config/PassConfigBase.h>
#include <../Includes/FrameStateForward.h>

// Forward declarations
class FrameStateBase;
class FrameStateService;
class MeshAsset;
class RenderStateService;
struct RenderStateData;
class CameraManager;
class ShaderManager;
class Lighting;
class Water;
class World;
class Skybox;
class RenderPassConfig;
class ViewportPassConfig;

class FrameStateService : public IManager, public ILifeCycle
{
public:
    FrameStateService();
    ~FrameStateService();

    HRESULT Init() override;
    void Render() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    const std::string& GetManagerName() const override {
        static const std::string name = "FrameStateService";
        return name;
    }
    static const std::string& GetStaticManagerName()
    {
        static const std::string name = "FrameStateService";
        return name;
    }
        
    FrameStateService* GetFrameStateService() {
        return m_frameStateService.get();
    };

	// Operacionesbásicas con los estados de Frame
    void ResetState(std::string stateName);
    void UpdateState(std::string stateName);

    // Acceso a los estados del Frame
	TerrainFrameState* TerrainState(bool FromReadBuffer = true);
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
    PipelineFrameState* PipelineState(bool FromReadBuffer = true);

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

    const std::shared_ptr<PassConfigBase> GetRenderPassConfig() const { return m_renderPassConfig; }
	void SetRenderPassConfig(const std::shared_ptr<PassConfigBase> config) { m_renderPassConfig = config; }

	// Métodos para bloqueo y desbloqueo explícito y controlado de buffers
    void SwapBuffers();
    void Lock();
    void Unlock();

private:	
    std::shared_ptr<MeshAsset> m_mesh = nullptr;
    std::shared_ptr<PassConfigBase> m_renderPassConfig; // Configuración de los pases de renderizado

    std::shared_ptr<Lighting> m_lighting; // Servicio para gestionar cámaras
    std::shared_ptr<CameraManager> m_cameraManager; // Servicio para gestionar cámaras
	std::shared_ptr<ShaderManager> m_shaderManager; // Servicio para gestionar los shaders
    std::shared_ptr<World> m_world; // Servicio para gestionar el estado de renderizado    
    std::shared_ptr<Water> m_water; // Servicio de agua
	std::shared_ptr<Skybox> m_skybox; // Servicio de skybox

    std::shared_ptr<RenderStateService> m_renderStateService; // Servicio para gestionar el estado de renderizado    
	std::shared_ptr<FrameStateService> m_frameStateService; // Servicio para gestionar el estado de frame
};