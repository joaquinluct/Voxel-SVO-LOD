#include "RenderTargetManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(RenderTargetManagerConfig,"RenderTargetManagerConfig");

RenderTargetManagerConfig::RenderTargetManagerConfig(){
	clear_color = {
		0.0f,
		0.2f,
		0.4f,
		1.0f
	};
	dependencies = {
		"DeviceManager"
	};
};
