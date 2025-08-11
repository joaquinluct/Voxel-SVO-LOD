#include "SkyboxMeshConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(SkyboxMeshConfig,"SkyboxMeshConfig");

SkyboxMeshConfig::SkyboxMeshConfig(){
	enabled = true;
	type = "MeshAsset";
	meshType = 1;
	name = "SkyboxMesh";
	mesh_path = "c:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Mesh\\skybox1.obj";
	texture = "DefaultSkybox";
	shader = "SkyboxShader";
	cast_shadows = false;
	receive_shadows = false;
	drawType = 1;
	render_passes = 16;
	dependencies = {
		"MeshAsset"
	};
};
