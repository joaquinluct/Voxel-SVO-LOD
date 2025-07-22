#include "KeyboardConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(KeyboardConfig,"KeyboardConfig");

KeyboardConfig::KeyboardConfig(){
	enabled = true;
	dependencies = {
		"KeyBindings"
	};
};
