#include "SkyboxConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(SkyboxConfig,"SkyboxConfig");

SkyboxConfig::SkyboxConfig(){
	name = "Skybox";
	enabled = true;
	version = "1.0.0";
	type = "DefaultSkybox";
	mesh = "SkyboxMesh";
	dependencies = {
		"SkyboxMesh"
	};
};
