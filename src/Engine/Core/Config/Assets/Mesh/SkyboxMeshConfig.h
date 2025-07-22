#pragma once

#include <string>
#include <vector>
#include <map>
#include <MeshAssetConfigBase.h>

class SkyboxMeshConfig : public MeshAssetConfigBase {
public:
	SkyboxMeshConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "SkyboxMeshConfig";
		return className;
	}
	bool enabled;
};
