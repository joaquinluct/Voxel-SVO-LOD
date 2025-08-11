#include "BoxMeshConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(BoxMeshConfig,"BoxMeshConfig");

BoxMeshConfig::BoxMeshConfig(){
	enabled = true;
	type = "MeshAsset";
	meshType = 1;
	name = "BoxMesh";
	mesh_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Mesh\\box.obj";
	texture = "TextureBricks001";
	shader = "PBRShadowShader";
	position = {
		0.0f,
		100.0f,
		0.0f
	};
	cast_shadows = true;
	receive_shadows = true;
	shader_shadows = "ShadowMapShader";
	drawType = 1;
	render_passes = 3;
	dependencies = {
		"MeshAsset"
	};
};
