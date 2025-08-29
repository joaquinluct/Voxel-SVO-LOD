#include "DeviceManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(DeviceManagerConfig,"DeviceManagerConfig");

DeviceManagerConfig::DeviceManagerConfig(){
	vsync = true;
	fullscreen = false;
	debug_device = true;
	swap_chain_buffer_count = 2;
	dependencies = {
		"None"
	};
};
