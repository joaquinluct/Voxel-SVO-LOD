#include "ShaderTextUIConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ShaderTextUIConfig,"ShaderTextUIConfig");

ShaderTextUIConfig::ShaderTextUIConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\TextShader.hlsl";
	vertex_def = "TextVertex";
	matrix_slots = {
		"MatrixBufferTypeOrthographic"
	};
	name = "ShaderTextUI";
	dependencies = {
		"ShaderAsset"
	};
};
