#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class PBRShaderConfig : public IAssetShaderConfig {
public:
	PBRShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "PBRShaderConfig";
		return className;
	}
};
