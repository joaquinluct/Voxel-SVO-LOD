#include "Terrain2ShaderConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(Terrain2ShaderConfig,"Terrain2ShaderConfig");

Terrain2ShaderConfig::Terrain2ShaderConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\Terrain2Shader.hlsl";
	vertex_def = "TextureMapVertex";
	needs_shadow = true;
	matrix_slots = {
		"MatrixBufferType",
		"DirectionalLight",
		"CameraData",
		"MaterialData",
		"TextureTransformations",
		"LightSpaceMatrices",
		"Terrain2BlendBuffer"
	};
	sampler_slots = {
		"baseSampler",
		"ShadowMapSampler"
	};
	name = "Terrain2Shader";
	dependencies = {
		"ShaderAsset"
	};
};
