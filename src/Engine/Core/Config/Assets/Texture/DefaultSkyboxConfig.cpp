#include "DefaultSkyboxConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(DefaultSkyboxConfig,"DefaultSkyboxConfig");

DefaultSkyboxConfig::DefaultSkyboxConfig(){
	type = "TextureAsset";
	map = "DefaultSkyboxMap";
	rotation_speed = 0.05f;
	dependencies = {
		"TextureAsset"
	};
};
