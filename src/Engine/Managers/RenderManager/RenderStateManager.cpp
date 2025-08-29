#include "FrameStateService.h"
#include <Locators/Registers/REGISTER_MANAGER_MACRO.h>
#include <CameraManager.h>
#include <Services/RenderStateService.h>
#include <Game/Systems/World.h>
#include <Game/Systems/Lighting.h>
#include <Game/Systems/Water.h>
#include <Game/Systems/Skybox.h>
#include <ServiceLocator/ServiceLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <ShaderManager.h>
#include <Assets/Base/MeshAsset.h>
#include <../Includes/FrameStates.h>
#include <Util/Utils.h>
#include <Util/DateTime.h>

REGISTER_MANAGER_TYPE(FrameStateService, "FrameStateService")
FrameStateService::FrameStateService()
{
}

FrameStateService::~FrameStateService()
{
}

HRESULT FrameStateService::Init()
{
    m_cameraManager = ManagerLocator::GetCameraManager();
    if (!m_cameraManager)
    {
        return E_FAIL;
	}    
	m_shaderManager = ManagerLocator::GetShaderManager();
    if (!m_shaderManager)
    {
        return E_FAIL;
    }    
	m_renderStateService = ServiceLocator::GetService<RenderStateService>();
    if (!m_renderStateService)
    {
        return E_FAIL;
	}
	m_frameStateService = ServiceLocator::GetService<FrameStateService>();
    if (!m_frameStateService)
    {
		return E_FAIL;
	}
    m_lighting = ServiceLocator::GetService<Lighting>();
    if (!m_lighting)
    {
        return E_FAIL;
    }
	m_world = ServiceLocator::GetService<World>();
    if (!m_world)
    {
        return E_FAIL;
    }
    m_water = ServiceLocator::GetService<Water>();
    if (!m_water)
    {
        return E_FAIL;
    }

    return S_OK;
}

void FrameStateService::Render()
{
    // Método vacío.
}

FrameStateBase* FrameStateService::UpdateTerrainState(float deltaTime) {
    TerrainFrameState* terrain = m_frameStateService->TerrainState(false);
    terrain->SetChunks(m_world->GetTerrain()->GetChunks(m_cameraManager->GetCurrentCamera()));
    return terrain;
}
FrameStateBase* FrameStateService::UpdateCameraState(float deltaTime) {
    CameraFrameState* camera = m_frameStateService->CameraState(false);
    camera->SetCameraPosition(m_cameraManager->GetCurrentCameraPosition());
    camera->SetViewMatrix(m_cameraManager->GetCurrentViewMatrix(true));
    camera->SetProjectionMatrix(m_cameraManager->GetCurrentProjectionMatrix(true));
    return camera;
}
FrameStateBase* FrameStateService::UpdateLightState(float deltaTime) {
    LightFrameState* light = m_frameStateService->LightingState(false);
    light->SetLightDirection(m_lighting->GetLightDirection());
    light->SetLightColor(m_lighting->GetLightColor());
    return light;
}
FrameStateBase* FrameStateService::UpdateWaterState(float deltaTime) {    
    WaterFrameState * water = m_frameStateService->WaterState(false);
    water->SetWaterData(WaterDefinition::GetTemporalWaterData());
    water->SetWaterTileTransformMatrices(m_water->GetWaterMatrices());
    return water;
}
FrameStateBase* FrameStateService::UpdateCommonState(float deltaTime) {
    CommonFrameState* common = m_frameStateService->CommonState(false);
    //common->SetWorldMatrix(this->GetMesh()->GetWorldMatrix());
    common->SetWorldMatrix(DirectX::XMMatrixIdentity());
    return common;
}
FrameStateBase* FrameStateService::UpdateMaterialState(float deltaTime) {
	const MeshAsset* mesh = this->GetMesh();
    if (!mesh || !mesh->GetMaterial()) {
        return nullptr;
	}
    MaterialFrameState* material = m_frameStateService->MaterialState(false);
	material->SetConstantBuffers(this->GetMesh()->GetMaterial()->GetConstantBuffers());
	material->SetNumTextures(this->GetMesh()->GetMaterial()->GetNumTextures());
	material->SetTextureTransform(this->GetMesh()->GetMaterial()->GetTextureTranforms());    
    return material;
}
FrameStateBase* FrameStateService::UpdateSkyboxState(float deltaTime) {
    SkyboxFrameState* skyBox = m_frameStateService->SkyboxState(false);
    if (!m_skybox) {
		return nullptr;
    }
    skyBox->SetSkyColor(m_skybox->GetSkyColor());
	skyBox->SetSunColor(m_skybox->GetSunColor());
    return skyBox;
}
FrameStateBase* FrameStateService::UpdateTimeState(float deltaTime) {
    TimeFrameState* time = m_frameStateService->TimeState(false);
    return time;
}
FrameStateBase* FrameStateService::UpdateMeshState(float deltaTime) {
    /*MeshFrameState* mesh = m_frameStateService->MeshState(false);
    mesh->SetMeshAsset(this->GetMesh());    
    return mesh;*/

	// Estamos al final del Update de todos los servicios y managers, por lo que el mesh debe ser reseteado.
    MeshFrameState* mesh = m_frameStateService->MeshState(false);
    mesh->SetMeshAsset(nullptr);
	return mesh;
}
FrameStateBase* FrameStateService::UpdateDeviceState(float deltaTime) {
    DeviceFrameState* device = m_frameStateService->DeviceState(false);
    device->SetClearColor({});    
    return device;
}
FrameStateBase* FrameStateService::UpdatePassState(float deltaTime) {
    PassFrameState* pass = m_frameStateService->PassState(false);    
    return pass;
}
FrameStateBase* FrameStateService::UpdateViewportState(float deltaTime) {
    ViewportFrameState* viewport = m_frameStateService->ViewportState(false);
    if (!viewport->IsInitialized()) {
        viewport->SetViewports(m_frameStateService->PipelineState(false)->GetAllViewports());
	}
    return viewport;
}
FrameStateBase* FrameStateService::UpdateShaderState(float deltaTime) {
    // Estamos al final del Update de todos los servicios y managers, por lo que el shader debe ser re-seteado.    
	ShaderFrameState* shaderState = m_frameStateService->ShaderState(false);
    shaderState->ClearMatrices();
    shaderState->SetShaderName("");
    return shaderState;
}
FrameStateBase* FrameStateService::UpdatePipelineState(float deltaTime) {
    PipelineFrameState* pipeline = m_frameStateService->PipelineState(false);    
    return pipeline;
}

void FrameStateService::SetMesh(const std::shared_ptr<MeshAsset>& mesh){
	MeshFrameState* meshState = m_frameStateService->MeshState();
	meshState->SetMeshAsset(mesh.get());
	m_frameStateService->SetData(FRAME_STATE_MESH, meshState);
	//m_frameStateService->FlushAndFreeze(FRAME_STATE_MESH);
    m_mesh = mesh; 
}

// Resets de los estados
// Normalmente al final del Update:
//     Unos actualizan con nuevos datos en este momento.
//     Otros resetean su estado para que no queden datos de un frame a otro.
FrameStateBase* FrameStateService::ResetMeshState(float deltaTime) {
    MeshFrameState* mesh = m_frameStateService->MeshState(false);
    mesh->SetMeshAsset(nullptr);
    return mesh;
}
FrameStateBase* FrameStateService::ResetShaderState(float deltaTime) {
    ShaderFrameState* shaderState = m_frameStateService->ShaderState(false);
    shaderState->ClearMatrices();
    shaderState->SetShaderName("");
    return shaderState;
}

void FrameStateService::Update(float deltaTime)
{
	// ACTUALIZAR ESTADOS QUE LO REQUIEREN
    FrameStateBase* state = UpdatePassState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_PASS, state);
    }
    state = UpdateCameraState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_CAMERA, state);
    }
    state = UpdateLightState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_LIGHTING, state);
	}
    state = UpdateWaterState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_WATER, state);
    }
    state = UpdateCommonState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_COMMON, state);
    }
    state = UpdateMaterialState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_MATERIAL, state);
    }
    state = UpdateSkyboxState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_SKYBOX, state);
    }
    state = UpdateTimeState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_TIME, state);
    }    
    state = UpdateDeviceState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_DEVICE, state);
    }    
    state = UpdateTerrainState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_TERRAIN, state);
    }

	// RESET DE LOS ESTADOS QUE LO REQUIEREN
    state = ResetMeshState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_TERRAIN, state);
    }
    state = ResetShaderState(deltaTime);
    if (state) {
        m_frameStateService->SetData(FRAME_STATE_TERRAIN, state);
    }

	// ACTUALIZAR EL ESTADO DE LOS BUFFERS
    SwapBuffers();
}

//std::function<void()> FrameStateService::GetUpdateFunctionByStateName(std::string stateName, float deltaTime) {
    /*if (stateName == FRAME_STATE_TERRAIN) {
        return [this, deltaTime]() { this->UpdateTerrainState(deltaTime); };
    }
    else if (stateName == FRAME_STATE_CAMERA) {
		return [this, deltaTime]() { this->UpdateCameraState(deltaTime); };
    }
    else if (stateName == FRAME_STATE_LIGHTING) {
		return [this, deltaTime]() { this->UpdateLightState(deltaTime); };
    }
    else if (stateName == FRAME_STATE_WATER) {
		return [this, deltaTime]() { this->UpdateWaterState(deltaTime); };
    }
    else if (stateName == FRAME_STATE_COMMON) {
		return [this, deltaTime]() { this->UpdateCommonState(deltaTime); };
    }
    else if (stateName == FRAME_STATE_MATERIAL) {
		return [this, deltaTime]() { this->UpdateMaterialState(deltaTime); };   
    }
    else if (stateName == FRAME_STATE_SKYBOX) {
		return [this, deltaTime]() { this->UpdateSkyboxState(deltaTime); };
    }
    else if (stateName == FRAME_STATE_TIME) {
		return [this, deltaTime]() { this->UpdateTimeState(deltaTime); };
    }
    else if (stateName == FRAME_STATE_MESH) {
		return [this, deltaTime]() { this->UpdateMeshState(deltaTime); };
    }
    else if (stateName == FRAME_STATE_DEVICE) {
		return [this, deltaTime]() { this->UpdateDeviceState(deltaTime); };
    }
    else if (stateName == FRAME_STATE_PASS) {
		return [this, deltaTime]() { this->UpdatePassState(deltaTime); };
    }
    else if (stateName == FRAME_STATE_VIEWPORT) {
		return [this, deltaTime]() { this->UpdateViewportState(deltaTime); };
    }
    else if (stateName == FRAME_STATE_SHADER) {
		return [this, deltaTime]() { this->UpdateShaderState(deltaTime); };
    }*/
    //return nullptr;
//}

void FrameStateService::ResetState(std::string stateName) {
}
void FrameStateService::UpdateState(std::string stateName) {
    //auto func = GetUpdateFunctionByStateName(stateName, 0.2f);

}

// Getters de lectura de los estados
TerrainFrameState* FrameStateService::TerrainState(bool FromReadBuffer) {
	return m_frameStateService->TerrainState(FromReadBuffer);
}
CameraFrameState* FrameStateService::CameraState(bool FromReadBuffer) {
	return m_frameStateService->CameraState(FromReadBuffer);
}
LightFrameState* FrameStateService::LightState(bool FromReadBuffer) {
	return m_frameStateService->LightingState(FromReadBuffer);
}
WaterFrameState* FrameStateService::WaterState(bool FromReadBuffer) {
	return m_frameStateService->WaterState(FromReadBuffer);
}
CommonFrameState* FrameStateService::CommonState(bool FromReadBuffer) {
	return m_frameStateService->CommonState(FromReadBuffer);
}
MaterialFrameState* FrameStateService::MaterialState(bool FromReadBuffer) {
	return m_frameStateService->MaterialState(FromReadBuffer);
}
SkyboxFrameState* FrameStateService::SkyboxState(bool FromReadBuffer) {
	return m_frameStateService->SkyboxState(FromReadBuffer);
}
TimeFrameState* FrameStateService::TimeState(bool FromReadBuffer) {
	return m_frameStateService->TimeState(FromReadBuffer);
}
MeshFrameState* FrameStateService::MeshState(bool FromReadBuffer) {
	return m_frameStateService->MeshState(FromReadBuffer);
}
DeviceFrameState* FrameStateService::DeviceState(bool FromReadBuffer) {
	return m_frameStateService->DeviceState(FromReadBuffer);
}
PassFrameState* FrameStateService::PassState(bool FromReadBuffer) {
	return m_frameStateService->PassState(FromReadBuffer);
}
ViewportFrameState* FrameStateService::ViewportState(bool FromReadBuffer) {
    return m_frameStateService->ViewportState(FromReadBuffer);
}
ShaderFrameState* FrameStateService::ShaderState(bool FromReadBuffer) {
    return m_frameStateService->ShaderState(FromReadBuffer);
}
PipelineFrameState* FrameStateService::PipelineState(bool FromReadBuffer) {
    return m_frameStateService->PipelineState(FromReadBuffer);
}

// Operaciones de gestión de los buffers
void FrameStateService::SwapBuffers() {
    //m_renderStateService->SwapBuffers();
	m_frameStateService->SwapBuffers();
}

void FrameStateService::Lock() {
	m_frameStateService->LockAll();
	//m_renderStateService->Lock();
}

void FrameStateService::Unlock() {
	m_frameStateService->UnlockAll();
    //m_renderStateService->Unlock();
}

void FrameStateService::Shutdown()
{
    m_mesh = nullptr;
    m_lighting = nullptr;
    m_cameraManager = nullptr;
    m_world = nullptr;
    m_water = nullptr;
    m_renderStateService = nullptr;
    m_frameStateService = nullptr;    
}