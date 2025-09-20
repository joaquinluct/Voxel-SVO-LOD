#include "ChronosConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ChronosConfig,"ChronosConfig");

ChronosConfig::ChronosConfig(){
	name = "ChronosService";
	enabled = true;
	version = "1.0.0";
	startDate = "01/08/2025 10:00:00";
	dependencies = {
		"None"
	};
};
