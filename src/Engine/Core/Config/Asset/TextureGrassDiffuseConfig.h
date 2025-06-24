// Configuración de TextureGrassDiffuse
#pragma once

#include <string>
#include <vector>

namespace TextureGrassDiffuseConfig{

	struct Values {
		std::string type = "TextureAsset";
		std::string file_path = "Assets/Textures/grass_diffuse.png";
		bool srgb = true;
	};
};
