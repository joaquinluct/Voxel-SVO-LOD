#include "MouseConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(MouseConfig,"MouseConfig");

MouseConfig::MouseConfig(){
	enabled = true;
	dependencies = {
		"DeviceManager"
	};
};
