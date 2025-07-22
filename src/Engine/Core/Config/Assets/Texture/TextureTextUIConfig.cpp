#include "TextureTextUIConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureTextUIConfig,"TextureTextUIConfig");

TextureTextUIConfig::TextureTextUIConfig(){
	type = "TextureAsset";
	map = "TextureTextUIMap";
	dependencies = {
		"DeviceManager",
		"TextureAsset"
	};
};
