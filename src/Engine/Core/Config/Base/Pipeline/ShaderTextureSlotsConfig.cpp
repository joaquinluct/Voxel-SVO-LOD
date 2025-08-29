#include "ShaderTextureSlotsConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ShaderTextureSlotsConfig,"ShaderTextureSlotsConfig");

ShaderTextureSlotsConfig::ShaderTextureSlotsConfig(){
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
