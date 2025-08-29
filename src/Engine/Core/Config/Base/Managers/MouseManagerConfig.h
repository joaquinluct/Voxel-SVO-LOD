#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class MouseManagerConfig : public ConfigBase {
public:
	MouseManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "MouseManagerConfig";
		return className;
	}
};
