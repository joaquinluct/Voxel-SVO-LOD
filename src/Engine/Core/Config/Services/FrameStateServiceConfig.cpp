#include "FrameStateServiceConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(FrameStateServiceConfig,"FrameStateServiceConfig");

FrameStateServiceConfig::FrameStateServiceConfig(){
	enabled = true;
	dependencies = {
		"None"
	};
};
