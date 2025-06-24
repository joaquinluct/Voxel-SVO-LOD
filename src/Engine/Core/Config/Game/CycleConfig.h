// Configuración de Cycle
#pragma once

#include <string>
#include <vector>

namespace CycleConfig{

	struct Values {
		std::string name = "CycleManager";
		bool active = true;
		std::string version = "1.0.0";
		std::string type = "DayNightCycle";
		bool day_night_cycle = true;
		float day_night_cycle_speed = 0.100000f;
	};
};
