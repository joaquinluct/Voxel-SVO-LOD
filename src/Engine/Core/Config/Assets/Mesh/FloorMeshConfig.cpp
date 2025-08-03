#include "FloorMeshConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(FloorMeshConfig,"FloorMeshConfig");

FloorMeshConfig::FloorMeshConfig(){
	enabled = true;
	type = "MeshAsset";
	name = "FloorMesh";
	mesh_path = "c:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Mesh\\floor.obj";
	texture = "TextureGrassDiffuse";
	texture_transforms = {
		3.0f,
		3.0f,
		0.0f,
		0.0f
	};
	shader = "PBRShader";
	position = {
		1.0f,
		100.0f,
		1.0f
	};
	cast_shadows = true;
	receive_shadows = true;
	dependencies = {
		"MeshAsset"
	};
};
