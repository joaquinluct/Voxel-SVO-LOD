#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class WheatherConfig : public ConfigService {
public:
	WheatherConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "WheatherConfig";
		return className;
	}
	std::string name;
	std::string version;
	float rain_intensity;
	float snow_intensity;
	float fog_density;
};
