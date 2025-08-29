#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class TextureSlotsConfig : public ConfigBase {
public:
	TextureSlotsConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TextureSlotsConfig";
		return className;
	}
	std::vector<std::string> slots;
};
