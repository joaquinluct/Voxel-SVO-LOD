#include "UITextConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(UITextConfig,"UITextConfig");

UITextConfig::UITextConfig(){
	enabled = true;
	dependencies = {
		"DeviceManager"
	};
};
