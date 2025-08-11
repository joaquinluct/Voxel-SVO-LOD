#include "DefaultShaderConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(DefaultShaderConfig,"DefaultShaderConfig");

DefaultShaderConfig::DefaultShaderConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\BaseShader.hlsl";
	vertex_def = "SimpleVertex";
	matrix_slots = {
		"MatrixBufferType"
	};
	sampler_slots = {
		"RegularSampler"
	};
	needs_shadow = false;
	name = "DefaultShader";
	dependencies = {
		"ShaderAsset"
	};
};
