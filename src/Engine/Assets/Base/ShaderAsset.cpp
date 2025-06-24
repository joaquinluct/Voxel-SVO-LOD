#include "ShaderAsset.h"
#include <REGISTER_ASSET_MACRO.h>
#include <Material/includes/VertexDefinition.h>
#include <vector>

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

void ShaderAsset::Shutdown() {

}