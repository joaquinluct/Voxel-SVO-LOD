#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class ShaderTextureSlotsConfig : public ConfigBase {
public:
	ShaderTextureSlotsConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ShaderTextureSlotsConfig";
		return className;
	}
	std::vector<std::string> slots;
};
