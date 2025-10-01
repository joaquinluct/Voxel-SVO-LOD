#include "BaseIndexConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(BaseIndexConfig,"BaseIndexConfig");

BaseIndexConfig::BaseIndexConfig(){
	index = {
		"ShaderAsset",
		"TextureAsset",
		"VertexAsset",
		"TerrainAsset",
		"MeshAsset",
		"UIAsset"
	};
	post_init = {
		"none"
	};
};
