// Configuración de Terrain
#pragma once

#include <string>
#include <vector>

namespace TerrainConfig{

	struct Values {
		std::string name = "TerrainManager";
		bool active = true;
		std::string version = "1.0.0";
		std::string type = "ProceduralTerrain";
		int seed = 1234567890;
	};
};
