#pragma once

#include <string>
#include <vector>
#include <map>
#include <ITextureConfig.h>

class TextureGrassDiffuseConfig : public ITextureConfig {
public:
	TextureGrassDiffuseConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TextureGrassDiffuseConfig";
		return className;
	}
};
