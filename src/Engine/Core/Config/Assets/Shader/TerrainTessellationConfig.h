#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class TerrainTessellationConfig : public IAssetShaderConfig {
public:
	TerrainTessellationConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TerrainTessellationConfig";
		return className;
	}
};
