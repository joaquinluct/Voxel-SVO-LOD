#include "WaterConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(WaterConfig,"WaterConfig");

WaterConfig::WaterConfig(){
	name = "Water";
	enabled = true;
	version = "1.0.0";
	type = "BasicWater";
	waterLevel = -2.0f;
	waveIntensity = 0.5f;
	waveSpeed = 1.0f;
	dependencies = {
		"Terrain"
	};
};
