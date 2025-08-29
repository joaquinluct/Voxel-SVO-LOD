#include "TextureSlotsConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureSlotsConfig,"TextureSlotsConfig");

TextureSlotsConfig::TextureSlotsConfig(){
	slots = {
		"albedoTexture",
		"normalTexture",
		"roughnessTexture",
		"metallicTexture",
		"aoTexture",
		"ShadowMap",
		"terrainTextures",
		"fontTexture"
	};
};
