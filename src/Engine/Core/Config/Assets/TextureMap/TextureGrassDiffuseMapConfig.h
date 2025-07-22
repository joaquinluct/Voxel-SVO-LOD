#pragma once

#include <string>
#include <vector>
#include <map>
#include <ITextureMapConfig.h>

class TextureGrassDiffuseMapConfig : public ITextureMapConfig {
public:
	TextureGrassDiffuseMapConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TextureGrassDiffuseMapConfig";
		return className;
	}
};
