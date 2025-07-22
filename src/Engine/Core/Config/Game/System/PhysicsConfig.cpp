#include "PhysicsConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(PhysicsConfig,"PhysicsConfig");

PhysicsConfig::PhysicsConfig(){
	name = "PhysicManager";
	enabled = true;
	version = "1.0.0";
	type = "BasicPhysics";
	gravity = {
		0.0f,
		-9.81f,
		0.0f
	};
};
