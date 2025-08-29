#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class MainIndexConfig : public ConfigBase {
public:
	MainIndexConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "MainIndexConfig";
		return className;
	}
	std::vector<std::string> mainIndex;
	std::vector<std::string> post_init;
};
