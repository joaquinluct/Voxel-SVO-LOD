#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class TerrainChunkEngineConfig : public ConfigService {
public:
	TerrainChunkEngineConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TerrainChunkEngineConfig";
		return className;
	}
	std::string name;
	std::string version;
	std::string description;
	int seed;
	float chunk_size;
	int chunk_radius;
	float world_width;
	float world_height;
	float terrain_height_mult;
	float ocean_level;
};
