#include "RenderManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(RenderManagerConfig,"RenderManagerConfig");

RenderManagerConfig::RenderManagerConfig(){
	passes = {
		"ShadowPass",
		"SkyboxPass",
		"MainColorPass",
		"UIPass"
	};
	dependencies = {
		"RenderTargetManager",
		"InitManager",
		"CameraManager",
		"Lighting"
	};
};
