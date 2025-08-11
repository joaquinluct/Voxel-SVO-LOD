#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class ProceduralSkyboxShaderConfig : public IAssetShaderConfig {
public:
	ProceduralSkyboxShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ProceduralSkyboxShaderConfig";
		return className;
	}
};
