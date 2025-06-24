// Configuración de RenderTargetManager
#pragma once

#include <string>
#include <vector>

namespace RenderTargetManagerConfig{

	struct Values {
		std::vector<float> clear_color {
			0.000000f,
			0.200000f,
			0.400000f,
			1.000000f,
		};
	};
};
