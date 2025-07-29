#include "BiomeConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(BiomeConfig,"BiomeConfig");

BiomeConfig::BiomeConfig(){
	name = "BiomeService";
	enabled = true;
	version = "1.0.0";
	type = "ForestBiome";
	vegetation_density = 0.8f;
	wildlife_density = 0.5f;
};
