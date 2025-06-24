// Configuración de UIManager
#pragma once

#include <string>
#include <vector>

namespace UIManagerConfig{

	struct Values {
		std::string initial_screen = "main_menu.yaml";
		std::string font_path = "Assets/Fonts/Roboto-Regular.ttf";
		int font_size = 24;
	};
};
