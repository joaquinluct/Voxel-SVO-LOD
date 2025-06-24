// Configuración de DefaultSkybox
#pragma once

#include <string>
#include <vector>
#include <IAssetSkyboxConfig.h>

namespace DefaultSkyboxConfig{

	struct Values : public IAssetSkyboxConfig {
		Values() {
			this->type = "SkyboxAsset";
			this->cubemap_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Skybox\\defaultSkybox.dds";
			this->mesh_name = "SkyboxCube";
			this->shader_name = "DefaultShader";
			this->rotation_speed = 0.050000f;
		};
	};
};
