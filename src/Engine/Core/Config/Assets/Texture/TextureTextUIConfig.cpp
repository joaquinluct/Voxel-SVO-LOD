#include "TextureTextUIConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureTextUIConfig,"TextureTextUIConfig");

TextureTextUIConfig::TextureTextUIConfig(){
	type = "TextureAsset";
	map = "TextureTextUIMap";
	texture_desc = "Texture2D_Simple_Jpg";
	x_scale = 1.0f;
	y_scale = 1.0f;
	x_offset = 0.0f;
	y_offset = 0.0f;
	dependencies = {
		"DeviceManager",
		"TextureAsset"
	};
};
