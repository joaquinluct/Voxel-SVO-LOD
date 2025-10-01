#include "WorldConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(WorldConfig,"WorldConfig");

WorldConfig::WorldConfig(){
	name = "WorldMananger";
	enabled = true;
	version = "1.0.0";
	description = "Un mundo abierto.";
	gameType = 1;
	gameFlags = 7;
	type = "VoxelWorld";
	respawnTime = 5;
	dependencies = {
		"Chronos",
		"Terrain",
		"UITextMesh"
	};
};
