#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class UITextConfig : public ConfigService {
public:
	UITextConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "UITextConfig";
		return className;
	}
};
