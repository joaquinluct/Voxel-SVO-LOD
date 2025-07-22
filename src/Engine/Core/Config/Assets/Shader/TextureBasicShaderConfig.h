#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class TextureBasicShaderConfig : public IAssetShaderConfig {
public:
	TextureBasicShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TextureBasicShaderConfig";
		return className;
	}
};
