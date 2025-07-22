#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class UIDebugConfig : public ConfigService {
public:
	UIDebugConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "UIDebugConfig";
		return className;
	}
};
