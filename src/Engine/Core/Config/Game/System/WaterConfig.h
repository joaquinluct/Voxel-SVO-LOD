#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class WaterConfig : public ConfigService {
public:
	WaterConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "WaterConfig";
		return className;
	}
	std::string name;
	std::string version;
	float waterLevel;
	float waveIntensity;
	float waveSpeed;
};
