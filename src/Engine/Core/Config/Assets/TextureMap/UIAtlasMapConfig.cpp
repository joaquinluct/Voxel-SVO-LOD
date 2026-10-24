#include "UIAtlasMapConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(UIAtlasMapConfig,"UIAtlasMapConfig");

UIAtlasMapConfig::UIAtlasMapConfig(){
	texture_type = "PNG";
	albedo = "Assets\\Textures\\UI\\atlas_ui_sdf.png";
};
