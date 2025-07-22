#include "SkyboxMeshConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(SkyboxMeshConfig,"SkyboxMeshConfig");

SkyboxMeshConfig::SkyboxMeshConfig(){
	enabled = true;
	type = "MeshAsset";
	name = "SkyboxMesh";
	mesh_path = "c:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Mesh\\skybox1.obj";
	texture = "DefaultSkybox";
	shader = "SkyboxShader";
	dependencies = {
		"MeshAsset"
	};
};
