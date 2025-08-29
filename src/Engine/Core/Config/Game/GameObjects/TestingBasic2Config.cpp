#include "TestingBasic2Config.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TestingBasic2Config,"TestingBasic2Config");

TestingBasic2Config::TestingBasic2Config(){
	name = "TestingBasicManager";
	enabled = false;
	shadow_pass = true;
	version = "1.0.0";
	type = "TestingBasic2";
	description = "Un triángulo de pruebas básicas.";
	dependencies = {
		"DeviceManager",
		"ShaderManager",
		"CameraManager"
	};
};
