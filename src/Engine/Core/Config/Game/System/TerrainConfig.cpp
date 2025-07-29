#include "TerrainConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TerrainConfig,"TerrainConfig");

TerrainConfig::TerrainConfig(){
	name = "TerrainService";
	enabled = true;
	version = "1.0.0";
	type = "ProceduralTerrain";
	seed = 1234567890;
};
