// Configuración de AssetManager
#pragma once

#include <string>
#include <vector>

namespace AssetManagerConfig{

	struct Values {
		std::string manifest_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Resources\\assets_manifest.yaml";
		int asset_cache_size = 1000;
		float asset_load_timeout = 5.000000f;
	};
};
