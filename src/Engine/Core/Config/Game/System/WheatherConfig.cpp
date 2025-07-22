#include "WheatherConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(WheatherConfig,"WheatherConfig");

WheatherConfig::WheatherConfig(){
	name = "WheatherManager";
	enabled = true;
	version = "1.0.0";
	type = "DynamicWeather";
	rain_intensity = 0.3f;
	snow_intensity = 0.1f;
	fog_density = 0.2f;
};
