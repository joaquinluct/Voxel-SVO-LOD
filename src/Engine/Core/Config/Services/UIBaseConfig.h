#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class UIBaseConfig : public ConfigService {
public:
	UIBaseConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "UIBaseConfig";
		return className;
	}
};
