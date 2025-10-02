#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class BiomeConfig : public ConfigService {
public:
	BiomeConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "BiomeConfig";
		return className;
	}
	std::string name;
	std::string version;
	int biome_type;
	float ocean_level;
	float max_height;
	float min_height;
	std::vector<std::string> biomes;
};
