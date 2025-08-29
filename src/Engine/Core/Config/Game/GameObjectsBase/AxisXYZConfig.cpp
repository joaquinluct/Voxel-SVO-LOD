#include "AxisXYZConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(AxisXYZConfig,"AxisXYZConfig");

AxisXYZConfig::AxisXYZConfig(){
	name = "AxisManager";
	enabled = false;
	type = "AxisXYZ";
	dependencies = {
		"DeviceManager",
		"ShaderManager",
		"CameraManager"
	};
};
