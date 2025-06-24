// Configuración de Biome
#pragma once

#include <string>
#include <vector>

namespace BiomeConfig{

	struct Values {
		std::string name = "BiomwManager";
		bool active = true;
		std::string version = "1.0.0";
		std::string type = "ForestBiome";
		float vegetation_density = 0.800000f;
		float wildlife_density = 0.500000f;
	};
};
