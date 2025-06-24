// Configuración de Wheather
#pragma once

#include <string>
#include <vector>

namespace WheatherConfig{

	struct Values {
		std::string name = "WheatherManager";
		bool active = true;
		std::string version = "1.0.0";
		std::string type = "DynamicWeather";
		float rain_intensity = 0.300000f;
		float snow_intensity = 0.100000f;
		float fog_density = 0.200000f;
	};
};
