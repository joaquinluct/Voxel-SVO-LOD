#pragma once

#include <string>
#include <vector>
#include <ConfigBase.h>

class VertexAssetConfigBase : public ConfigBase
{
public:
	std::string name;
	std::vector<uint16_t> GetIndex() const { return {}; };
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "VertexAssetConfigBase";
		return className;
	}
};
