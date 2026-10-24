#include "ShaderTextSDFConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ShaderTextSDFConfig,"ShaderTextSDFConfig");

ShaderTextSDFConfig::ShaderTextSDFConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\TextSDF.hlsl";
	vertex_def = "TextVertex";
	matrix_slots = {
		"MatrixBufferTypeOrthographic"
	};
	sampler_slots = {
		"UISampler"
	};
	needs_shadow = false;
	name = "ShaderTextSDF";
	dependencies = {
		"ShaderAsset"
	};
};
