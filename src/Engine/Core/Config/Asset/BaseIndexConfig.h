// Configuración de BaseIndex
#pragma once

#include <string>
#include <vector>

namespace BaseIndexConfig{

	struct Values {
		std::vector<std::string> index {
			"ShaderAsset",
			"TextureAsset",
			"MeshAsset",
			"SkyboxAsset",
		};
	};
};
