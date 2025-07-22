#include "TextureGrassDiffuseConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureGrassDiffuseConfig,"TextureGrassDiffuseConfig");

TextureGrassDiffuseConfig::TextureGrassDiffuseConfig(){
	type = "TextureAsset";
	map = "TextureGrassDiffuseMap";
	dependencies = {
		"DeviceManager",
		"TextureAsset"
	};
};
