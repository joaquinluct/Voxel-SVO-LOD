#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class DefaultShaderConfig : public IAssetShaderConfig {
public:
	DefaultShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "DefaultShaderConfig";
		return className;
	}
};
