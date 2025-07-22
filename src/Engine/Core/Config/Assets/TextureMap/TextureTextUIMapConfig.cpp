#include "TextureTextUIMapConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureTextUIMapConfig,"TextureTextUIMapConfig");

TextureTextUIMapConfig::TextureTextUIMapConfig(){
	texture_type = "JPG";
	albedo = "Assets\\Textures\\UI\\DejaVuSansMono.jpg";
};
