#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class KeyboardManagerConfig : public ConfigBase {
public:
	KeyboardManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "KeyboardManagerConfig";
		return className;
	}
	std::string service;
	std::map<std::string, std::vector<std::string>> key_bindings;
};
