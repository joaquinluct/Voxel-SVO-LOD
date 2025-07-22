#include "TestingFloorConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TestingFloorConfig,"TestingFloorConfig");

TestingFloorConfig::TestingFloorConfig(){
	name = "TestingFloor";
	enabled = true;
	version = "1.0.0";
	type = "TestingFloor";
	description = "Un suelo de pruebas.";
	dependencies = {
		"FloorMesh"
	};
};
