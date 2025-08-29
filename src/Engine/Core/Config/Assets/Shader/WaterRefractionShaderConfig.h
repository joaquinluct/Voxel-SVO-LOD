#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class WaterRefractionShaderConfig : public IAssetShaderConfig {
public:
	WaterRefractionShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "WaterRefractionShaderConfig";
		return className;
	}
};
