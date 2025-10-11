#include "TerrainTessellationConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TerrainTessellationConfig,"TerrainTessellationConfig");

TerrainTessellationConfig::TerrainTessellationConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\TerrainTessellation.hlsl";
	vertex_def = "SimpleVertex";
	matrix_slots = {
		"MatrixBufferType",
		"CameraData"
	};
	sampler_slots = {
		"baseSampler"
	};
	needs_shadow = false;
	name = "TerrainTessellation";
	dependencies = {
		"ShaderAsset"
	};
};
