#include "MouseManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(MouseManagerConfig,"MouseManagerConfig");

MouseManagerConfig::MouseManagerConfig(){
	dependencies = {
		"Mouse"
	};
};
