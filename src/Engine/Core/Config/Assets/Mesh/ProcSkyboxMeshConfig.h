#pragma once

#include <string>
#include <vector>
#include <map>
#include <MeshAssetConfigBase.h>

class ProcSkyboxMeshConfig : public MeshAssetConfigBase {
public:
	ProcSkyboxMeshConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ProcSkyboxMeshConfig";
		return className;
	}
	bool enabled;
};
