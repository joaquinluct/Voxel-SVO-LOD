#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class MouseConfig : public ConfigService {
public:
	MouseConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "MouseConfig";
		return className;
	}
};
