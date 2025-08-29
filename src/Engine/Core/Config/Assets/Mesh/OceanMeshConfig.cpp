#include "OceanMeshConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(OceanMeshConfig,"OceanMeshConfig");

OceanMeshConfig::OceanMeshConfig(){
	enabled = true;
	type = "MeshAsset";
	meshType = 1;
	name = "WaterMesh";
	mesh_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Mesh\\water.obj";
	texture = "none";
	shader = "OceanShader";
	position = {
		0.0f,
		0.0f,
		0.0f
	};
	cast_shadows = false;
	receive_shadows = false;
	shader_shadows = "none";
	drawType = 3;
	render_passes = 896;
	dependencies = {
		"MeshAsset"
	};
};
