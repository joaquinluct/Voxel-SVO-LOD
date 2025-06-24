// Configuración de Engine
#pragma once

#include <string>
#include <vector>

namespace EngineConfig{

	struct Values {
		std::vector<std::string> managers_init_order {
			"DeviceManager",
			"KeyboardManager",
			"CameraManager",
			"RenderTargetManager",
			"ShaderManager",
			"WorldMatrixManager",
			"AssetManager",
			"GameManager",
			"UIManager",
		};
		std::vector<std::string> managers_render_order {
			"RenderTargetManager",
			"WorldMatrixManager",
			"ShaderManager",
			"GameManager",
			"UIManager",
			"DeviceManager",
		};
		std::vector<std::string> managers_update_order {
			"KeyboardManager",
			"WorldMatrixManager",
			"CameraManager",
			"GameManager",
			"UIManager",
		};
	};
};
