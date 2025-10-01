#include "ProceduralTerrainConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ProceduralTerrainConfig,"ProceduralTerrainConfig");

ProceduralTerrainConfig::ProceduralTerrainConfig(){
	enabled = true;
	type = "TerrainAsset";
	meshType = 3;
	name = "ProceduralTerrain";
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
	render_passes = 33;
	dependencies = {
		"ShaderManager",
		"TerrainAsset",
		"Terrain2Shader",
		"TerrainMesh"
	};
};
