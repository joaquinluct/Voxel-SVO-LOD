#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class BaseIndexConfig : public ConfigBase {
public:
	BaseIndexConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "BaseIndexConfig";
		return className;
	}
	std::vector<std::string> index;
};
