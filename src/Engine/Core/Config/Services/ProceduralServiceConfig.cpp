#include "ProceduralServiceConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ProceduralServiceConfig,"ProceduralServiceConfig");

ProceduralServiceConfig::ProceduralServiceConfig(){
	enabled = true;
	dependencies = {
		"None"
	};
};
