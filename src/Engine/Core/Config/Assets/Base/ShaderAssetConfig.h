#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class ShaderAssetConfig : public ConfigBase {
public:
	ShaderAssetConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ShaderAssetConfig";
		return className;
	}
};
