#include "PBRShadowShaderConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(PBRShadowShaderConfig,"PBRShadowShaderConfig");

PBRShadowShaderConfig::PBRShadowShaderConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\PBRShadowShader.hlsl";
	vertex_def = "TextureMapVertex";
	matrix_slots = {
		"MatrixBufferType",
		"DirectionalLight",
		"CameraData",
		"MaterialData",
		"LightSpaceMatrices"
	};
	name = "PBRShadowShader";
	dependencies = {
		"ShaderAsset"
	};
};
