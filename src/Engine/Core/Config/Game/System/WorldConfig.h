#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class WorldConfig : public ConfigService {
public:
	WorldConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "WorldConfig";
		return className;
	}
	std::string name;
	std::string version;
	std::string description;
	int gameType;
	int gameFlags;
	int respawnTime;
};
