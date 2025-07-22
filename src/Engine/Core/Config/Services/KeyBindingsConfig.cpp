#include "KeyBindingsConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(KeyBindingsConfig,"KeyBindingsConfig");

KeyBindingsConfig::KeyBindingsConfig(){
	enabled = true;
};
