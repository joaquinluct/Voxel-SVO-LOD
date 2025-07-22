#include "RenderManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(RenderManagerConfig,"RenderManagerConfig");

RenderManagerConfig::RenderManagerConfig(){
	dependencies = {
		"UIManager"
	};
};
