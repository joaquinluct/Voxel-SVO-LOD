#include "PBRShaderConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(PBRShaderConfig,"PBRShaderConfig");

PBRShaderConfig::PBRShaderConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\PBRShader.hlsl";
	vertex_def = "TextureMapVertex";
	matrix_slots = {
		"MatrixBufferType",
		"DirectionalLight",
		"CameraData",
		"MaterialData",
		"TextureTransformations"
	};
	name = "PBRShader";
	dependencies = {
		"ShaderAsset"
	};
};
