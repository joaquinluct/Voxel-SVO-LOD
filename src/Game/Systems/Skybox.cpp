#include "Skybox.h"
#include <REGISTER_SERVICE_MACRO.h>
#include <Defines/VertexDefinition.h>
#include <Util/Utils.h>

REGISTER_SERVICE_TYPE(Skybox, "Skybox")

Skybox::Skybox()
{
}

Skybox::~Skybox()
{
}

HRESULT Skybox::Init() 
{
	std::shared_ptr<MeshAsset> sharedMesh = std::static_pointer_cast<MeshAsset>(AssetLocator::GetAsset("SkyboxMesh"));
	if (!sharedMesh) {
		return E_FAIL; // Return failure if mesh asset is not found
	}

	mesh = *sharedMesh;

	mesh.Init();

	return S_OK; // Return success
}

void Skybox::Update(float deltaTime)
{
	// Update logic for the skybox manager
}

void Skybox::Render()
{	
	mesh.Render();
}

void Skybox::Shutdown(){}