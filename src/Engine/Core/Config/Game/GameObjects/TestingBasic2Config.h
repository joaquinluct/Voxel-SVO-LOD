#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class TestingBasic2Config : public ConfigService {
public:
	TestingBasic2Config();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TestingBasic2Config";
		return className;
	}
	std::string name;
	std::string version;
	std::string description;
};
