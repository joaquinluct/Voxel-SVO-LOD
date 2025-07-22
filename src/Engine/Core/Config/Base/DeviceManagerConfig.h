#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class DeviceManagerConfig : public ConfigBase {
public:
	DeviceManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "DeviceManagerConfig";
		return className;
	}
	bool vsync;
	bool fullscreen;
	bool debug_device;
	int swap_chain_buffer_count;
};
