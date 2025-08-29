#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class ChronosConfig : public ConfigService {
public:
	ChronosConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ChronosConfig";
		return className;
	}
	std::string name;
	std::string version;
	std::string startDate;
};
