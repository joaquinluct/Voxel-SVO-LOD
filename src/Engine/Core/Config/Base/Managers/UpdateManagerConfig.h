#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class UpdateManagerConfig : public ConfigBase {
public:
	UpdateManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "UpdateManagerConfig";
		return className;
	}
};
