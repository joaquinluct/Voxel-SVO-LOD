#include "SkyboxShaderConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(SkyboxShaderConfig,"SkyboxShaderConfig");

SkyboxShaderConfig::SkyboxShaderConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\SkyboxShader.hlsl";
	vertex_def = "SkyboxVertex";
	matrix_slots = {
		"MatrixBufferTypeSkyBox"
	};
	name = "SkyboxShader";
	dependencies = {
		"ShaderAsset"
	};
};
