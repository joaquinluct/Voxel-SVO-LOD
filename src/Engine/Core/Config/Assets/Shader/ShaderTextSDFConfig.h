#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class ShaderTextSDFConfig : public IAssetShaderConfig {
public:
	ShaderTextSDFConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ShaderTextSDFConfig";
		return className;
	}
};
