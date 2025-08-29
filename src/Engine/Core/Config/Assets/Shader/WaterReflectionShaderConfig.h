#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class WaterReflectionShaderConfig : public IAssetShaderConfig {
public:
	WaterReflectionShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "WaterReflectionShaderConfig";
		return className;
	}
};
