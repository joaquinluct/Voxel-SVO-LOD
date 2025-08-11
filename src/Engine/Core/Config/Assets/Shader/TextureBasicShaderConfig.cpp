#include "TextureBasicShaderConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureBasicShaderConfig,"TextureBasicShaderConfig");

TextureBasicShaderConfig::TextureBasicShaderConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\TextureBasicShader.hlsl";
	vertex_def = "TextureBasicVertex";
	matrix_slots = {
		"MatrixBufferType"
	};
	sampler_slots = {
		"RegularSampler"
	};
	needs_shadow = false;
	name = "TextureBasicShader";
	dependencies = {
		"ShaderAsset"
	};
};
