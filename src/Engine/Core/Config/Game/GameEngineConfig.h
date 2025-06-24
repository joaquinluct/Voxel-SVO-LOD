// Configuración de GameEngine
#pragma once

#include <string>
#include <vector>

namespace GameEngineConfig{

	struct Values {
		std::vector<std::string> services_init_order {};
		std::vector<std::string> services_render_order {};
	};
};
