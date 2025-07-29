#pragma once

#include <string>
#include <vector>
#include <map>
#include <ITextureConfig.h>

class TextureBricks001Config : public ITextureConfig {
public:
	TextureBricks001Config();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TextureBricks001Config";
		return className;
	}
};
