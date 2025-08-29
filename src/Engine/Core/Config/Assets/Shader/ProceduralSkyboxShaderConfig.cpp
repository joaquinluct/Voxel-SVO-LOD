#include "ProceduralSkyboxShaderConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ProceduralSkyboxShaderConfig,"ProceduralSkyboxShaderConfig");

ProceduralSkyboxShaderConfig::ProceduralSkyboxShaderConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\MyProceduralSkyboxShader.hlsl";
	vertex_def = "SimpleVertex";
	matrix_slots = {
		"MatrixBufferType",
		"ProceduralSkyboxMatrix"
	};
	sampler_slots = {
		"baseSampler"
	};
	needs_shadow = false;
	name = "ProceduralSkyboxShader";
	dependencies = {
		"ShaderAsset"
	};
};
