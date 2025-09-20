#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class ShadowsConfig : public ConfigService {
public:
	ShadowsConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ShadowsConfig";
		return className;
	}
	std::string name;
	std::string version;
};
