#include "OceanShaderConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(OceanShaderConfig,"OceanShaderConfig");

OceanShaderConfig::OceanShaderConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\OceanShader.hlsl";
	vertex_def = "SimpleVertex";
	needs_shadow = false;
	matrix_slots = {
		"MatrixBufferType",
		"DirectionalLight",
		"CameraData",
		"WaterData",
		"TimeData",
		"WaterInstance"
	};
	sampler_slots = {
		"baseSampler"
	};
	name = "OceanShader";
	dependencies = {
		"ShaderAsset"
	};
};
