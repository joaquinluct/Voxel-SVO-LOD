#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class ShaderMatrixSlotsConfig : public ConfigBase {
public:
	ShaderMatrixSlotsConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ShaderMatrixSlotsConfig";
		return className;
	}
	std::vector<std::string> slots;
};
