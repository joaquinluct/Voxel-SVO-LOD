#include "SkyboxConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(SkyboxConfig,"SkyboxConfig");

SkyboxConfig::SkyboxConfig(){
	name = "Skybox";
	enabled = true;
	shadow_pass = false;
	version = "1.0.0";
	type = "DefaultSkybox";
	mesh = "SkyboxMesh";
	sky_color = {
		0.4f,
		0.4f,
		0.9f,
		1.0f
	};
	sun_color = {
		1.0f,
		0.9f,
		0.6f,
		1.0f
	};
	dependencies = {
		"ProcSkyboxMesh",
		"RenderManager"
	};
};
