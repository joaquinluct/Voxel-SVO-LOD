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
	int geometry_type;
	int terrain_distribution;
	int terrain_generator;
	std::string geometry_engine;
	std::string terrain_engine;
	int procedural_engine;
	std::string geometry_lod_strategy;
	std::vector<int> geometry_lod_thresholds;
	bool geometry_lod_blending;
	std::string generation_type;
	int generation_algorithm;
	float generation_frequency;
	float generation_amplitude;
	int generation_octaves;
	float generation_lacunarity;
	float generation_gain;
	int geometry_resolution;
	bool geometry_normals;
	bool geometry_smooth;
	std::string spatial_structure;
	int quad_tree_max_depth;
	int quad_tree_min_size;
	std::string quad_tree_lod_strategy;
	std::string buffer_layout;
	int buffer_zones;
};
