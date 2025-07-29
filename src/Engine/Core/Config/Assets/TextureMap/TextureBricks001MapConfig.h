#pragma once

#include <string>
#include <vector>
#include <map>
#include <ITextureMapConfig.h>

class TextureBricks001MapConfig : public ITextureMapConfig {
public:
	TextureBricks001MapConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TextureBricks001MapConfig";
		return className;
	}
	std::string displacement;
};
