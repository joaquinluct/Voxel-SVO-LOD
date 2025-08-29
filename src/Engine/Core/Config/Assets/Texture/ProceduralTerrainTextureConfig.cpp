#include "ProceduralTerrainTextureConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ProceduralTerrainTextureConfig,"ProceduralTerrainTextureConfig");

ProceduralTerrainTextureConfig::ProceduralTerrainTextureConfig(){
	type = "TextureAsset";
	map = "ProceduralTerrainTextureMap";
	texture_desc = "Texture2D_Array_Jpg";
	x_scale = 1.0f;
	y_scale = 1.0f;
	x_offset = 0.0f;
	y_offset = 0.0f;
	dependencies = {
		"DeviceManager",
		"TextureAsset"
	};
};
