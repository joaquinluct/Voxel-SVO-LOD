#include "SkyboxCubeConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(SkyboxCubeConfig,"SkyboxCubeConfig");

SkyboxCubeConfig::SkyboxCubeConfig(){
	enabled = true;
	type = "VertexAsset";
	name = "SkyboxCube";
	dependencies = {
		"VertexAsset"
	};
};
