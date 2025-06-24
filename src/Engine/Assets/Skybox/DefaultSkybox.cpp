#include "DefaultSkybox.h"
#include <REGISTER_ASSET_MACRO.h>
#include <Material/includes/VertexDefinition.h>
#include <vector>

REGISTER_ASSET_TYPE(DefaultSkybox, "DefaultSkybox")

DefaultSkybox::DefaultSkybox() {}

DefaultSkybox::~DefaultSkybox() {
	// Cleanup if necessary
}

HRESULT DefaultSkybox::Init() {
	m_mesh = AssetLocator::GetAsset<MeshAsset>();
	/*std::vector<VertexDefinition::SimpleVertex> vertex{};
	std::vector<WORD> index{};
	m_mesh->CreateCubeData<VertexDefinition::SimpleVertex>(vertex, index);*/
	return S_OK;
}

void DefaultSkybox::GetMesh() {
	// Render the default skybox
	// This could involve binding textures and drawing a cube or sphere
}

void DefaultSkybox::Shutdown() {

}