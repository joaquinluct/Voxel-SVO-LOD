#pragma once

#include <string>
#include <vector>
#include <map>
#include <PassConfigBase.h>

class WaterPassConfig : public PassConfigBase {
public:
	WaterPassConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "WaterPassConfig";
		return className;
	}
	int id;
	bool enabled;
	std::string shader_default;
};
