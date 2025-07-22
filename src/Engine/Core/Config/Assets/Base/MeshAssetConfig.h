#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class MeshAssetConfig : public ConfigBase {
public:
	MeshAssetConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "MeshAssetConfig";
		return className;
	}
};
