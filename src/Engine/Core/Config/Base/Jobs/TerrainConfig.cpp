#include "TerrainConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TerrainConfig,"TerrainConfig");

TerrainConfig::TerrainConfig(){
	name = "Terrain";
	enabled = true;
	version = "1.0.0";
	flags = 7;
	seed = 1234567890;
	procedural_engine = 1;
	chunk_size = 128;
	initialRenderDistanceChunks = 30;
	worldWidth = 2048.0f;
	worldHeight = 2048.0f;
	terrainHeight = 2.0f;
	waterLevel = -2.0f;
	dependencies = {
		"ChunkService",
		"CameraManager",
		"ShaderManager",
		"TerrainAsset",
		"ProceduralTerrain",
		"MeshAsset"
	};
};
