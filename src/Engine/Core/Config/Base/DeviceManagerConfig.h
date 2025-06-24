// Configuración de DeviceManager
#pragma once

#include <string>
#include <vector>

namespace DeviceManagerConfig{

	struct Values {
		bool vsync = true;
		bool fullscreen = false;
		bool debug_device = true;
		int swap_chain_buffer_count = 2;
	};
};
