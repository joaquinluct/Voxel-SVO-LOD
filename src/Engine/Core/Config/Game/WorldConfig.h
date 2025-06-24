// Configuración de World
#pragma once

#include <string>
#include <vector>

namespace WorldConfig{

	struct Values {
		std::string name = "WorldMananger";
		bool active = true;
		std::string version = "1.0.0";
		std::string type = "VoxelWorld";
		std::string description = "Un juego de aventura en un mundo voxelizado.";
		int respawnTime = 5;
	};
};
