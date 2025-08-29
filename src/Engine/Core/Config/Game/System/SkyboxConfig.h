#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class SkyboxConfig : public ConfigService {
public:
	SkyboxConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "SkyboxConfig";
		return className;
	}
	std::string name;
	std::string version;
	std::vector<float> sky_color;
	std::vector<float> sun_color;
};
