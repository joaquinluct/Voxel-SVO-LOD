#pragma once

#include <string>
#include <vector>
#include <map>
#include <ITextureConfig.h>

class DefaultSkyboxConfig : public ITextureConfig {
public:
	DefaultSkyboxConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "DefaultSkyboxConfig";
		return className;
	}
	float rotation_speed;
};
