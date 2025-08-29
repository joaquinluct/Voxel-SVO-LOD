#include "TerrainShaderConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TerrainShaderConfig,"TerrainShaderConfig");

TerrainShaderConfig::TerrainShaderConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\TerrainShader.hlsl";
	vertex_def = "TextureMapVertex";
	needs_shadow = true;
	matrix_slots = {
		"MatrixBufferType",
		"DirectionalLight",
		"CameraData",
		"MaterialData",
		"TextureTransformations",
		"LightSpaceMatrices",
		"TerrainBlendBuffer"
	};
	sampler_slots = {
		"baseSampler",
		"ShadowMapSampler"
	};
	name = "TerrainShader";
	dependencies = {
		"ShaderAsset"
	};
};
