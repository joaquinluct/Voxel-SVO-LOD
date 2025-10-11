#include "TerrainConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TerrainConfig,"TerrainConfig");

TerrainConfig::TerrainConfig(){
	name = "Terrain";
	enabled = true;
	version = "1.0.0";
	type = 1;
	geometry_type = 1;
	terrain_distribution = 1;
	terrain_generator = 2;
	geometry_engine = "GeometryChunkEngine";
	terrain_engine = "TerrainChunkEngine";
	procedural_engine = 1;
	geometry_lod_strategy = "Distance";
	geometry_lod_thresholds = {
		0,
		500,
		1000,
		2000,
		4000
	};
	geometry_lod_blending = true;
	generation_type = "Procedural";
	generation_algorithm = 1;
	generation_frequency = 0.01f;
	generation_amplitude = 1.0f;
	generation_octaves = 4;
	generation_lacunarity = 2.0f;
	generation_gain = 0.5f;
	geometry_resolution = 1;
	geometry_normals = true;
	geometry_smooth = false;
	spatial_structure = "Chunk";
	quad_tree_max_depth = 0;
	quad_tree_min_size = 0;
	quad_tree_lod_strategy = "None";
	buffer_layout = "Unified";
	buffer_zones = 1;
	dependencies = {
		"ChunkService",
		"CameraManager",
		"ShaderManager",
		"TerrainAsset",
		"ProceduralTerrain",
		"MeshAsset"
	};
};
