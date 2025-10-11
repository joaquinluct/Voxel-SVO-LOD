#include "GeometryChunkEngineConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(GeometryChunkEngineConfig,"GeometryChunkEngineConfig");

GeometryChunkEngineConfig::GeometryChunkEngineConfig(){
	name = "GeometryChunkEngine";
	enabled = true;
	version = "1.0.0";
	terrain_asset = "ProceduralTerrain";
	description = "Motor de geometría basado en chunks.";
	dependencies = {
		"DeviceManager",
		"TerrainAsset"
	};
};
