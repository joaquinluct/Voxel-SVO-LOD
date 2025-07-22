#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class VertexAssetConfig : public ConfigBase {
public:
	VertexAssetConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "VertexAssetConfig";
		return className;
	}
};
