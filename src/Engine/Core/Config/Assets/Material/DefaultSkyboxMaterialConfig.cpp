#include "DefaultSkyboxMaterialConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(DefaultSkyboxMaterialConfig,"DefaultSkyboxMaterialConfig");

DefaultSkyboxMaterialConfig::DefaultSkyboxMaterialConfig(){
	type = "MaterialAsset";
	shader = "SkyboxShader";
	textures = {
		"DefaultSkybox"
	};
	dependencies = {
		"CameraManager",
		"MaterialAsset"
	};
};
