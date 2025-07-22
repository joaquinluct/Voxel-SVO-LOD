#include "MainIndexConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(MainIndexConfig,"MainIndexConfig");

MainIndexConfig::MainIndexConfig(){
	mainIndex = {
		"SkyboxShader",
		"DefaultShader",
		"ShaderTextUI",
		"TextureBasicShader",
		"TextureMapShader",
		"PBRShader",
		"FloorVertex",
		"FloorMesh",
		"TextureTextUI",
		"TextureGrassDiffuse",
		"DefaultSkybox",
		"SkyboxCube",
		"SkyboxMesh"
	};
};
