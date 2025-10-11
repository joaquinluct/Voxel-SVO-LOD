#include "TerrainChunkEngineConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TerrainChunkEngineConfig,"TerrainChunkEngineConfig");

TerrainChunkEngineConfig::TerrainChunkEngineConfig(){
	name = "TerrainChunkEngine";
	enabled = true;
	version = "1.0.0";
	description = "Motor de terreno basado en chunks";
	seed = 1234567890;
	chunk_size = 128.0f;
	chunk_radius = 8;
	world_width = 163840.0f;
	world_height = 163840.0f;
	terrain_height_mult = 3.164f;
	ocean_level = -2.0f;
	dependencies = {
		"DeviceManager",
		"TerrainAsset"
	};
};
