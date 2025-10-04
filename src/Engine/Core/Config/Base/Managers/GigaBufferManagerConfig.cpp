#include "GigaBufferManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(GigaBufferManagerConfig,"GigaBufferManagerConfig");

GigaBufferManagerConfig::GigaBufferManagerConfig(){
	dependencies = {
		"DeviceManager"
	};
};
