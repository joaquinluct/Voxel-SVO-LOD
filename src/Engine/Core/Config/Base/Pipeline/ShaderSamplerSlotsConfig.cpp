#include "ShaderSamplerSlotsConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ShaderSamplerSlotsConfig,"ShaderSamplerSlotsConfig");

ShaderSamplerSlotsConfig::ShaderSamplerSlotsConfig(){
	slots = {
		"baseSampler",
		"ShadowMapSampler",
		"fontSampler"
	};
};
