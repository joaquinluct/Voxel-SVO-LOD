#pragma once

#include <string>
#include <vector>
#include <map>
#include <MeshAssetConfigBase.h>

class OceanMeshConfig : public MeshAssetConfigBase {
public:
	OceanMeshConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "OceanMeshConfig";
		return className;
	}
	bool enabled;
};
