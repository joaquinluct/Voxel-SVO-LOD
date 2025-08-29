#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class Terrain2ShaderConfig : public IAssetShaderConfig {
public:
	Terrain2ShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "Terrain2ShaderConfig";
		return className;
	}
};
