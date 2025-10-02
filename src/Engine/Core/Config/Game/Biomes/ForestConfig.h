#pragma once

#include <string>
#include <vector>
#include <map>
#include <BiomeBaseConfig.h>

class ForestConfig : public BiomeBaseConfig {
public:
	ForestConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ForestConfig";
		return className;
	}
	float zone1_max_ratio;
	float zone2_max_ratio;
};
