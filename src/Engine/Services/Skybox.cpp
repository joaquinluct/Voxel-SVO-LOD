#include "Skybox.h"
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(Skybox, "Skybox")

Skybox::Skybox()
{
}

Skybox::~Skybox()
{
}

HRESULT Skybox::Init() 
{
	m_config = new SkyboxConfig::Values();

	if (!m_config->active) {
		return S_OK;
	}

	// SkyboxAsset
	std::shared_ptr<SkyboxAsset> skyAsset = AssetLocator::GetAsset<SkyboxAsset>();
	skyAsset->SetType(m_config->type);
	// CubBoxAsset
	
	// MeshAsset
	m_mesh = AssetLocator::GetAsset<MeshAsset>();
	IAssetSkyboxConfig* skyConfig = skyAsset->GetConfig();
	m_mesh->SetType(skyConfig->mesh_name);
	m_mesh->SetTexture(skyConfig->cubemap_path);
	m_mesh->SetShaderName(skyConfig->shader_name);
	m_deviceManager = ManagerLocator::GetManager<DeviceManager>();
			
	return S_OK;
}

void Skybox::Update(float deltaTime)
{
	// Update logic for the skybox manager
}

void Skybox::Render()
{	
	ID3D11DeviceContext* context = m_deviceManager->GetContext();

	if (!context || !m_mesh)  {
		return; // Ensure context is valid before proceeding
	}

	m_mesh->Render();
	// Render logic for the skybox manager
}

void Skybox::Shutdown()
{
	// Release resources for the skybox manager
}