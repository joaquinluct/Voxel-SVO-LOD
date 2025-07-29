#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class PBRShadowShaderConfig : public IAssetShaderConfig {
public:
	PBRShadowShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "PBRShadowShaderConfig";
		return className;
	}
};
