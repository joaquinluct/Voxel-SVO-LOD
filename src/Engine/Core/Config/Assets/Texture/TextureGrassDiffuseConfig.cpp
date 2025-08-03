#include "TextureGrassDiffuseConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureGrassDiffuseConfig,"TextureGrassDiffuseConfig");

TextureGrassDiffuseConfig::TextureGrassDiffuseConfig(){
	type = "TextureAsset";
	map = "TextureGrassDiffuseMap";
	x_scale = 1.0f;
	y_scale = 1.0f;
	x_offset = 0.0f;
	y_offset = 0.0f;
	dependencies = {
		"DeviceManager",
		"TextureAsset"
	};
};
