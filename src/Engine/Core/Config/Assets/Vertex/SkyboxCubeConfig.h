#pragma once

#include <string>
#include <vector>
#include <map>
#include <VertexAssetConfigBase.h>

class SkyboxCubeConfig : public VertexAssetConfigBase {
public:
	SkyboxCubeConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "SkyboxCubeConfig";
		return className;
	}
	bool enabled;
};
