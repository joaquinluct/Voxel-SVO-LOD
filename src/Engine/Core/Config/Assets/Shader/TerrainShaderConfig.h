#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class TerrainShaderConfig : public IAssetShaderConfig {
public:
	TerrainShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TerrainShaderConfig";
		return className;
	}
};
