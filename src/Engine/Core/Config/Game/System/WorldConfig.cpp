#include "WorldConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(WorldConfig,"WorldConfig");

WorldConfig::WorldConfig(){
	name = "WorldMananger";
	enabled = true;
	version = "1.0.0";
	description = "Un juego de aventura en un mundo voxelizado.";
	type = "VoxelWorld";
	respawnTime = 5;
};
