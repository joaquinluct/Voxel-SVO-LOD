#include "InputManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(InputManagerConfig,"InputManagerConfig");

InputManagerConfig::InputManagerConfig(){
	dependencies = {
		"MouseManager",
		"KeyboardManager"
	};
};
