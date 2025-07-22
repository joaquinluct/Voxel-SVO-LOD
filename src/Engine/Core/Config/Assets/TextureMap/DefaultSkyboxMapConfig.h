#pragma once

#include <string>
#include <vector>
#include <map>
#include <ITextureMapConfig.h>

class DefaultSkyboxMapConfig : public ITextureMapConfig {
public:
	DefaultSkyboxMapConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "DefaultSkyboxMapConfig";
		return className;
	}
};
