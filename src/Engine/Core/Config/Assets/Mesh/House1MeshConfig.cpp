#include "House1MeshConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(House1MeshConfig,"House1MeshConfig");

House1MeshConfig::House1MeshConfig(){
	enabled = true;
	type = "MeshAsset";
	meshType = 1;
	name = "House1Mesh";
	mesh_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Mesh\\Builds\\Houses\\house1.obj";
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
