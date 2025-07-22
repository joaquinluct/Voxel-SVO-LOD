#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class KeyboardConfig : public ConfigService {
public:
	KeyboardConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "KeyboardConfig";
		return className;
	}
};
