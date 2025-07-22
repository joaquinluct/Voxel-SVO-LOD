#include "CycleConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(CycleConfig,"CycleConfig");

CycleConfig::CycleConfig(){
	name = "CycleManager";
	enabled = true;
	version = "1.0.0";
	type = "DayNightCycle";
	day_night_cycle = true;
	day_night_cycle_speed = 0.1f;
};
