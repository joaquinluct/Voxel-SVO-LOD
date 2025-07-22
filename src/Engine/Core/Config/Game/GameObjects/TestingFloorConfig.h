#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class TestingFloorConfig : public ConfigService {
public:
	TestingFloorConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TestingFloorConfig";
		return className;
	}
	std::string name;
	std::string version;
	std::string description;
};
