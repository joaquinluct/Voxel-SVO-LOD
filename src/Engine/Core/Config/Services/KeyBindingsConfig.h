#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class KeyBindingsConfig : public ConfigService {
public:
	KeyBindingsConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "KeyBindingsConfig";
		return className;
	}
};
