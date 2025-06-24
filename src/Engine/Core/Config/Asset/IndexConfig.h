// Configuración de Index
#pragma once

#include <string>
#include <vector>

namespace IndexConfig{

	struct Values {
		std::vector<std::string> index {
			"TextureGrassDiffuse",
			"MeshSimpleCube",
			"DefaultSkybox",
		};
	};
};
