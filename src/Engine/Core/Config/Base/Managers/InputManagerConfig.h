#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class InputManagerConfig : public ConfigBase {
public:
	InputManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "InputManagerConfig";
		return className;
	}
};
