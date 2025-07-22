#pragma once

#include <string>
#include <vector>
#include <map>
#include <MeshAssetConfigBase.h>

class FloorMeshConfig : public MeshAssetConfigBase {
public:
	FloorMeshConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "FloorMeshConfig";
		return className;
	}
	bool enabled;
};
