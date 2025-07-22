#pragma once

#include <string>
#include <vector>
#include <map>
#include <VertexAssetConfigBase.h>

class FloorVertexConfig : public VertexAssetConfigBase {
public:
	FloorVertexConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "FloorVertexConfig";
		return className;
	}
	bool enabled;
};
