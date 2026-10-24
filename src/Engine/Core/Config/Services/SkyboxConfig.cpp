#include "SkyboxConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(SkyboxConfig,"SkyboxConfig");

SkyboxConfig::SkyboxConfig(){
	enabled = true;
	shadow_pass = false;
	dependencies = {
		"RenderManager",
		"CameraManager",
		"Lighting"
	};
};
