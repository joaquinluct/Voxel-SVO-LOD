#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class PhysicsConfig : public ConfigService {
public:
	PhysicsConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "PhysicsConfig";
		return className;
	}
	std::string name;
	std::string version;
	std::vector<float> gravity;
};
