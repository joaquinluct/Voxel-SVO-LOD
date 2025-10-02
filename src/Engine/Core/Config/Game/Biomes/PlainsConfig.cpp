#include "PlainsConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(PlainsConfig,"PlainsConfig");

PlainsConfig::PlainsConfig(){
	id = 1;
	name = "Plains";
	type = 1;
	domain = 0;
	minHeight = 0.0f;
	maxHeight = 0.8f;
	zone1_max_ratio = 0.125f;
	zone2_max_ratio = 0.625f;
	neverNearOf = {
		"Tundra"
	};
	alwaysNearOf = {
		"Forest"
	};
	zone1_type = 6;
	textures_zone1 = {
		"Dry_Riverbed_4k",
		"RockyTrail_4k",
		"CoastSand_4k"
	};
	zone2_type = 2;
	textures_zone2 = {
		"RockAndGrass_4k",
		"RockyTrail_4k",
		"RockyTrail_4k"
	};
	zone3_type = 2;
	textures_zone3 = {
		"Mossy_Rock_4k",
		"RockyTrail_4k",
		"RockyTrail_4k"
	};
	canHaveBeaches = true;
	canHaveLakes = true;
	canHaveMountains = false;
	canHaveRivers = true;
	canHaveRoads = true;
	canHaveTrees = true;
};
