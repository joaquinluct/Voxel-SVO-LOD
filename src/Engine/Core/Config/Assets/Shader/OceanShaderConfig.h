#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class OceanShaderConfig : public IAssetShaderConfig {
public:
	OceanShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "OceanShaderConfig";
		return className;
	}
};
