#include "ShadowMapShaderConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ShadowMapShaderConfig,"ShadowMapShaderConfig");

ShadowMapShaderConfig::ShadowMapShaderConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\ShadowMapShader.hlsl";
	vertex_def = "SimpleVertex";
	matrix_slots = {
		"ShadowMapMatrices"
	};
	sampler_slots = {
		"None"
	};
	name = "ShadowMapShader";
	needs_shadow = false;
	dependencies = {
		"ShaderAsset"
	};
};
