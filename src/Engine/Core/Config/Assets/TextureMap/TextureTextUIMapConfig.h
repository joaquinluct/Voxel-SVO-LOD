#pragma once

#include <string>
#include <vector>
#include <map>
#include <ITextureMapConfig.h>

class TextureTextUIMapConfig : public ITextureMapConfig {
public:
	TextureTextUIMapConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TextureTextUIMapConfig";
		return className;
	}
};
