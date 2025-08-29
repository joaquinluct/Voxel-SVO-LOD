#pragma once

#include <string>
#include <vector>
#include <map>
#include <ITextureMapConfig.h>

class ProceduralTerrainTextureMapConfig : public ITextureMapConfig {
public:
	ProceduralTerrainTextureMapConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ProceduralTerrainTextureMapConfig";
		return className;
	}
};
