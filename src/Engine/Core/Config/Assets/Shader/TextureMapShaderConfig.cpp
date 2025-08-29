#include "TextureMapShaderConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureMapShaderConfig,"TextureMapShaderConfig");

TextureMapShaderConfig::TextureMapShaderConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\TextureMapShader.hlsl";
	vertex_def = "TextureMapVertex";
	matrix_slots = {
		"MatrixBufferType",
		"DirectionalLight",
		"CameraData",
		"MaterialData",
		"TextureTransformations"
	};
	sampler_slots = {
		"baseSampler"
	};
	needs_shadow = false;
	name = "TextureMapShader";
	dependencies = {
		"ShaderAsset"
	};
};
