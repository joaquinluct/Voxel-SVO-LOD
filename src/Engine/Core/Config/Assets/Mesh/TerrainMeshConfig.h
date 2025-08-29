#pragma once

#include <string>
#include <vector>
#include <map>
#include <MeshAssetConfigBase.h>

class TerrainMeshConfig : public MeshAssetConfigBase {
public:
	TerrainMeshConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TerrainMeshConfig";
		return className;
	}
	bool enabled;
};
