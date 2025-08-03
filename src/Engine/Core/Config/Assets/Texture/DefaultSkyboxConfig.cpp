#include "DefaultSkyboxConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(DefaultSkyboxConfig,"DefaultSkyboxConfig");

DefaultSkyboxConfig::DefaultSkyboxConfig(){
	type = "TextureAsset";
	map = "DefaultSkyboxMap";
	x_scale = 1.0f;
	y_scale = 1.0f;
	x_offset = 0.0f;
	y_offset = 0.0f;
	rotation_speed = 0.05f;
	dependencies = {
		"TextureAsset"
	};
};
