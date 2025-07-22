#include "LineConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(LineConfig,"LineConfig");

LineConfig::LineConfig(){
	name = "Line";
	enabled = true;
	type = "Line";
	dependencies = {
		"DeviceManager",
		"WorldMatrixManager",
		"ShaderManager",
		"CameraManager"
	};
};
