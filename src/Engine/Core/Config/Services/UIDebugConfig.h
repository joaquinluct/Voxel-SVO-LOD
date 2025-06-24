// Configuración de UIDebug
#pragma once

#include <string>
#include <vector>

namespace UIDebugConfig{

	struct Values {
		bool Enabled = true;
		std::string texture = "TextureTextUI";
		std::string shader = "ShaderTextUI";
		std::string text = "Debug Information";
	};
};
