#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class SkyboxShaderConfig : public IAssetShaderConfig {
public:
	SkyboxShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "SkyboxShaderConfig";
		return className;
	}
};
