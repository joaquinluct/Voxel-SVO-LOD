#include "ShaderAsset.h"
#include <REGISTER_ASSET_MACRO.h>
#include <Defines/VertexDefinition.h>
#include <vector>
#include <IAssetShaderConfig.h>

REGISTER_ASSET_TYPE(ShaderAsset, "ShaderAsset")

ShaderAsset::ShaderAsset() {}

ShaderAsset::~ShaderAsset() {
	// Cleanup if necessary
}

HRESULT ShaderAsset::Init() {
	//m_mesh = AssetLocator::GetAsset<MeshAsset>();
	/*std::vector<VertexDefinition::SimpleVertex> vertex{};
	std::vector<WORD> index{};
	m_mesh->CreateCubeData<VertexDefinition::SimpleVertex>(vertex, index);*/
	return S_OK;
}


void ShaderAsset::SetConfig(std::shared_ptr<ConfigBase> config) {
	m_shaderConfig = std::dynamic_pointer_cast<IAssetShaderConfig>(config);
}

std::shared_ptr<IAssetShaderConfig> ShaderAsset::GetConfig() const {
	return m_shaderConfig;
}

void ShaderAsset::SetName(const std::string& name) {
	m_name = name;
}

void ShaderAsset::Shutdown() {

}