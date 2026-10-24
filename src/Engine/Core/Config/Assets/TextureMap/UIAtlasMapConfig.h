#pragma once

#include <string>
#include <vector>
#include <map>
#include <ITextureMapConfig.h>

class UIAtlasMapConfig : public ITextureMapConfig {
public:
	UIAtlasMapConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "UIAtlasMapConfig";
		return className;
	}
};
