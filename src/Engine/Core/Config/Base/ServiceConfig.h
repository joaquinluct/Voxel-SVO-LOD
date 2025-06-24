// Configuración de Service
#pragma once

#include <string>
#include <vector>

namespace ServiceConfig{

	struct Values {
		std::vector<std::string> services_pre_init_order {
			"Keyboard",
		};
		std::vector<std::string> services_post_init_order {
			"UIDebug",
		};
		std::vector<std::string> services_render_order {
			"Keyboard",
		};
		std::vector<std::string> services_update_order {
			"Keyboard",
		};
	};
};
