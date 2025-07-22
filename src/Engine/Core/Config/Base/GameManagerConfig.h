#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class GameManagerConfig : public ConfigBase {
public:
	GameManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "GameManagerConfig";
		return className;
	}
	std::string manifestPath;
	std::string playerEntitConfig;
	std::vector<float> initialPlayerPosition;
	float gameSpeedMultiplier;
	bool debugMode;
};
