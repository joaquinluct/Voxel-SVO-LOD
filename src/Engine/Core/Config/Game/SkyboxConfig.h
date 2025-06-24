// Configuración de Skybox
#pragma once

#include <string>
#include <vector>

namespace SkyboxConfig{

	struct Values {
		std::string name = "Skybox";
		bool active = true;
		std::string version = "1.0.0";
		std::string type = "DefaultSkybox";
	};
};
