#pragma once

#include <string>
#include <vector>
#include <map>
#include <TerrainAssetConfigBase.h>

class ProceduralTerrainConfig : public TerrainAssetConfigBase {
public:
	ProceduralTerrainConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ProceduralTerrainConfig";
		return className;
	}
	bool enabled;
};
