#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class TessellationGeometryEngineConfig : public ConfigService {
public:
	TessellationGeometryEngineConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TessellationGeometryEngineConfig";
		return className;
	}
	std::string name;
	std::string version;
	std::string terrain_asset;
	std::string shader_name;
	std::string description;
};
