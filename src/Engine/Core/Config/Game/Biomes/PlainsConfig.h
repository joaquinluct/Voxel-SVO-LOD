#pragma once

#include <string>
#include <vector>
#include <map>
#include <BiomeBaseConfig.h>

class PlainsConfig : public BiomeBaseConfig {
public:
	PlainsConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "PlainsConfig";
		return className;
	}
	float zone1_max_ratio;
	float zone2_max_ratio;
};
