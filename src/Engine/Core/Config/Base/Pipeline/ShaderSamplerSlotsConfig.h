#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class ShaderSamplerSlotsConfig : public ConfigBase {
public:
	ShaderSamplerSlotsConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ShaderSamplerSlotsConfig";
		return className;
	}
	std::vector<std::string> slots;
};
