#include "SkyboxAsset.h"
#include <REGISTER_ASSET_MACRO.h>
#include <Material/includes/VertexDefinition.h>
#include <vector>

REGISTER_ASSET_TYPE(SkyboxAsset, "SkyboxAsset")

SkyboxAsset::SkyboxAsset() : m_config() {}

SkyboxAsset::~SkyboxAsset() {
	// Cleanup if necessary
}

HRESULT SkyboxAsset::Init() {
	m_mesh = AssetLocator::GetAsset<MeshAsset>();

	return S_OK;
}

void SkyboxAsset::SetType(const std::string& type) {
	if (type == "DefaultSkybox") {
		m_config = new DefaultSkyboxConfig::Values();
	}
}

void SkyboxAsset::GetMesh() {
	// Render the default skybox
	// This could involve binding textures and drawing a cube or sphere
}

void SkyboxAsset::Shutdown() {

}