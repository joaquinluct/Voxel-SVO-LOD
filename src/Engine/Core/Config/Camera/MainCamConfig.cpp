#include "MainCamConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(MainCamConfig,"MainCamConfig");

MainCamConfig::MainCamConfig(){
	type = "FirstPersonCamera";
	active = true;
	move_speed = 150.0f;
	rotation_speed = 120.0f;
	fov_y = 60.0f;
	near_clip = 0.1f;
	far_clip = 6000.0f;
	position = {
		0.0f,
		100.0f,
		0.0f
	};
	initial_pitch = 0.0f;
	initial_yaw = 0.0f;
};
