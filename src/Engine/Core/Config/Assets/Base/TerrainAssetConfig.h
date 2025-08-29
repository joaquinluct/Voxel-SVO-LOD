#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class TerrainAssetConfig : public ConfigBase {
public:
	TerrainAssetConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TerrainAssetConfig";
		return className;
	}
};
