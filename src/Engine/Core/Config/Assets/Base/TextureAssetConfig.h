#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class TextureAssetConfig : public ConfigBase {
public:
	TextureAssetConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TextureAssetConfig";
		return className;
	}
};
