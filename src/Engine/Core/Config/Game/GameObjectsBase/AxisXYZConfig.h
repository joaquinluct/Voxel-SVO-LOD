#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class AxisXYZConfig : public ConfigService {
public:
	AxisXYZConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "AxisXYZConfig";
		return className;
	}
	std::string name;
};
