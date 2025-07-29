#include "ShadowsConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ShadowsConfig,"ShadowsConfig");

ShadowsConfig::ShadowsConfig(){
	name = "Shadows";
	enabled = true;
	shadow_pass = false;
	version = "1.0.0";
	type = "Shadows";
	dependencies = {
		"ShaderManager"
	};
};
