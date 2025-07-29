#pragma once

#include <string>
#include <vector>
#include <map>
#include <MeshAssetConfigBase.h>

class House1MeshConfig : public MeshAssetConfigBase {
public:
	House1MeshConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "House1MeshConfig";
		return className;
	}
	bool enabled;
};
