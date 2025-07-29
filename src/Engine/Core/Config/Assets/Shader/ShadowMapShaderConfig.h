#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class ShadowMapShaderConfig : public IAssetShaderConfig {
public:
	ShadowMapShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ShadowMapShaderConfig";
		return className;
	}
};
