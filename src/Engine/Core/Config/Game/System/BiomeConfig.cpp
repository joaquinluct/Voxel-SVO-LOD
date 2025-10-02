#include "BiomeConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(BiomeConfig,"BiomeConfig");

BiomeConfig::BiomeConfig(){
	name = "Biome";
	enabled = true;
	version = "1.0.0";
	biome_type = 1;
	ocean_level = 0.0f;
	max_height = 2000.0f;
	min_height = -2000.0f;
	biomes = {
		"Plains",
		"Forest"
	};
};
