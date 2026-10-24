#pragma once

#include <string>
#include <vector>
#include <map>
#include <ITextureConfig.h>

class UIAtlasConfig : public ITextureConfig {
public:
	UIAtlasConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "UIAtlasConfig";
		return className;
	}
};
