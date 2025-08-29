#include "InitManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(InitManagerConfig,"InitManagerConfig");

InitManagerConfig::InitManagerConfig(){
	dependencies = {
		"Shadows"
	};
};
