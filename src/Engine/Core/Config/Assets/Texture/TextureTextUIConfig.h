#pragma once

#include <string>
#include <vector>
#include <map>
#include <ITextureConfig.h>

class TextureTextUIConfig : public ITextureConfig {
public:
	TextureTextUIConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TextureTextUIConfig";
		return className;
	}
};
