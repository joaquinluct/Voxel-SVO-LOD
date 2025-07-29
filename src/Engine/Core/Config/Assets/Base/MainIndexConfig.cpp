#include "MainIndexConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(MainIndexConfig,"MainIndexConfig");

MainIndexConfig::MainIndexConfig(){
	mainIndex = {
		"SkyboxShader",
		"DefaultShader",
		"ShaderTextUI",
		"TextureBasicShader",
		"ShadowMapShader",
		"TextureMapShader",
		"PBRShader",
		"FloorVertex",
		"FloorMesh",
		"House1Mesh",
		"TextureTextUI",
		"TextureGrassDiffuse",
		"TextureBricks001",
		"DefaultSkybox",
		"SkyboxCube",
		"SkyboxMesh"
	};
};
