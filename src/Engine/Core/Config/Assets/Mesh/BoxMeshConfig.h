#pragma once

#include <string>
#include <vector>
#include <map>
#include <MeshAssetConfigBase.h>

class BoxMeshConfig : public MeshAssetConfigBase {
public:
	BoxMeshConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "BoxMeshConfig";
		return className;
	}
	bool enabled;
};
