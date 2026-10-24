#include "UIAtlasConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(UIAtlasConfig,"UIAtlasConfig");

UIAtlasConfig::UIAtlasConfig(){
	type = "TextureAsset";
	map = "UIAtlasMap";
	texture_desc = "Texture2D_Simple_Png";
	x_scale = 1.0f;
	y_scale = 1.0f;
	x_offset = 0.0f;
	y_offset = 0.0f;
	dependencies = {
		"DeviceManager",
		"TextureAsset"
	};
};
