#include "DebugCamConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(DebugCamConfig,"DebugCamConfig");

DebugCamConfig::DebugCamConfig(){
	type = "FirstPersonCamera";
	active = false;
	move_speed = 500.0f;
	rotation_speed = 180.0f;
	fov_y = 75.0f;
	near_clip = 0.1f;
	far_clip = 10000.0f;
	position = {
		500.0f,
		1500.0f,
		500.0f
	};
};
