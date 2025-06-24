// Configuración de ShaderManager
#pragma once

#include <string>
#include <vector>

namespace ShaderManagerConfig{

	struct Values {
		bool shader_cache_enabled = true;
		std::vector<std::string> shader_paths {
			"Shaders/Core/",
			"Shaders/Materials/",
		};
	};
};
