#pragma once

#include <string>
#include <vector>
#include <map>
#include <ITextureConfig.h>

class ProceduralTerrainTextureConfig : public ITextureConfig {
public:
	ProceduralTerrainTextureConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ProceduralTerrainTextureConfig";
		return className;
	}
};
