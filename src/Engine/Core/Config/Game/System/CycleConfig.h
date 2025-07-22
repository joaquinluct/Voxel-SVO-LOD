#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class CycleConfig : public ConfigService {
public:
	CycleConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "CycleConfig";
		return className;
	}
	std::string name;
	std::string version;
	bool day_night_cycle;
	float day_night_cycle_speed;
};
