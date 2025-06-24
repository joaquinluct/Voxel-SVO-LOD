// Configuración de SkyboxCube
#pragma once

#include <string>
#include <vector>
#include <IAssetMeshConfig.h>

namespace SkyboxCubeConfig{

template <typename T>
	struct Values : public IAssetMeshConfig<T> {
		Values() {
			this->type = "MeshAsset";
			this->has_normals = true;
			this->has_indices = true;
			this->has_tangents = false;
			this->has_uvs = false;
			this->has_colors = false;
			this->has_bones = false;
		};
	};
};
