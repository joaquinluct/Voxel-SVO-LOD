#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class LineConfig : public ConfigService {
public:
	LineConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "LineConfig";
		return className;
	}
	std::string name;
};
