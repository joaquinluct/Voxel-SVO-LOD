#include "LightingConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(LightingConfig,"LightingConfig");

LightingConfig::LightingConfig(){
	name = "LightManager";
	enabled = true;
	version = "1.0.0";
	type = "DynamicLighting";
	ambient_light = {
		0.2f,
		0.2f,
		0.2f
	};
	directional_light = {
		1.0f,
		1.0f,
		1.0f
	};
	shadows = true;
};
