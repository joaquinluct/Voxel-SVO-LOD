#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class DefaultSkyboxMaterialConfig : public ConfigBase {
public:
	DefaultSkyboxMaterialConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "DefaultSkyboxMaterialConfig";
		return className;
	}
	std::string shader;
	std::vector<std::string> textures;
};
