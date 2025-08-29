#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class InitManagerConfig : public ConfigBase {
public:
	InitManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "InitManagerConfig";
		return className;
	}
};
