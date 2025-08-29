#include "TerrainMeshConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TerrainMeshConfig,"TerrainMeshConfig");

TerrainMeshConfig::TerrainMeshConfig(){
	enabled = true;
	type = "MeshAsset";
	meshType = 3;
	name = "TerrainMesh";
	texture = "ProceduralTerrainTexture";
	texture_transforms = {
		3.0f,
		3.0f,
		0.0f,
		0.0f
	};
	shader = "Terrain2Shader";
	position = {
		0.0f,
		100.0f,
		0.0f
	};
	cast_shadows = true;
	receive_shadows = true;
	shader_shadows = "ShadowMapShader";
	drawType = 1;
	render_passes = 417;
	dependencies = {
		"MeshAsset"
	};
};
