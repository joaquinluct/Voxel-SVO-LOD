#include "ForestConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ForestConfig,"ForestConfig");

ForestConfig::ForestConfig(){
	id = 2;
	name = "Forest";
	type = 1;
	domain = 0;
	minHeight = 0.0f;
	maxHeight = 1.0f;
	zone1_max_ratio = 0.20f;
	zone2_max_ratio = 0.70f;
	neverNearOf = {
		"Desert"
	};
	alwaysNearOf = {
		"Plains"
	};
	zone1_type = 5;
	textures_zone1 = {
		"ForestGround_A_4k",
		"ForestGround_B_4k",
		"ForestSand_4k"
	};
	zone2_type = 3;
	textures_zone2 = {
		"MossyRock_4k",
		"DirtPath_4k",
		"ForestRoots_4k"
	};
	zone3_type = 9;
	textures_zone3 = {
		"RockSlope_4k",
		"MossyCliff_4k",
		"PineNeedles_4k"
	};
	canHaveBeaches = true;
	canHaveLakes = true;
	canHaveMountains = false;
	canHaveRivers = true;
	canHaveRoads = true;
	canHaveTrees = true;
};
