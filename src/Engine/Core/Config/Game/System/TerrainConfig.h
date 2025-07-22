#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class TerrainConfig : public ConfigService {
public:
	TerrainConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TerrainConfig";
		return className;
	}
	std::string name;
	std::string version;
	int seed;
};
