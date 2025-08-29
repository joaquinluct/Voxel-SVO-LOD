#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class TerrainConfig : public ConfigService {
public:
	TerrainConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TerrainConfig";
		return className;
	}
	std::string name;
	std::string version;
	int flags;
	int seed;
	int procedural_engine;
	int chunk_size;
	int initialRenderDistanceChunks;
	float worldWidth;
	float worldHeight;
	float terrainHeight;
	float waterLevel;
};
