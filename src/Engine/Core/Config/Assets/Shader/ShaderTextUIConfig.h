#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class ShaderTextUIConfig : public IAssetShaderConfig {
public:
	ShaderTextUIConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ShaderTextUIConfig";
		return className;
	}
};
