#pragma once

#include <string>
#include <vector>
#include <map>
#include <IAssetShaderConfig.h>

class TextureMapShaderConfig : public IAssetShaderConfig {
public:
	TextureMapShaderConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TextureMapShaderConfig";
		return className;
	}
};
