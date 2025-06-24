// Configuración de Lighting
#pragma once

#include <string>
#include <vector>

namespace LightingConfig{

	struct Values {
		std::string name = "LightManager";
		bool active = true;
		std::string version = "1.0.0";
		std::string type = "DynamicLighting";
		std::vector<float> ambient_light {
			0.200000f,
			0.200000f,
			0.200000f,
		};
		std::vector<float> directional_light {
			1.000000f,
			1.000000f,
			1.000000f,
		};
		bool shadows = true;
	};
};
