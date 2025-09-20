#include "LightingConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(LightingConfig,"LightingConfig");

LightingConfig::LightingConfig(){
	name = "LightService";
	enabled = true;
	shadow_pass = false;
	version = "1.0.0";
	type = "DirectionalLight";
	ambient_light = {
		0.2f,
		0.2f,
		0.2f
	};
	directional_light = {
		0.5f,
		0.7f,
		0.5f
	};
	directional_light_color = {
		1.0f,
		1.0f,
		1.0f,
		1.0f
	};
	shadows = true;
	dependencies = {
		"KeyboardManager"
	};
};
