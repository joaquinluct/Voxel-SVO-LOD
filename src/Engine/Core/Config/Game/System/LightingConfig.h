#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class LightingConfig : public ConfigService {
public:
	LightingConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "LightingConfig";
		return className;
	}
	std::string name;
	std::string version;
	std::vector<float> ambient_light;
	std::vector<float> directional_light;
	bool shadows;
};
