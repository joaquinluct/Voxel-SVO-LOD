// Configuración de DefaultShader
#pragma once

#include <string>
#include <vector>
#include <IAssetShaderConfig.h>

namespace DefaultShaderConfig{

	struct Values : public IAssetShaderConfig {
		Values() {
			this->type = "ShaderAsset";
			this->shader_path = "Assets/Shaders/TextureShader.hlsl";
		};
	};
};
