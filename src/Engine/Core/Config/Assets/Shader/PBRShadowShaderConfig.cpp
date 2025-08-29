#include "PBRShadowShaderConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(PBRShadowShaderConfig,"PBRShadowShaderConfig");

PBRShadowShaderConfig::PBRShadowShaderConfig(){
	type = "ShaderAsset";
	shader_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Shader\\PBRShadowShader.hlsl";
	vertex_def = "TextureMapVertex";
	needs_shadow = true;
	matrix_slots = {
		"MatrixBufferType",
		"DirectionalLight",
		"CameraData",
		"MaterialData",
		"TextureTransformations",
		"LightSpaceMatrices"
	};
	sampler_slots = {
		"baseSampler",
		"ShadowMapSampler"
	};
	name = "PBRShadowShader";
	dependencies = {
		"ShaderAsset"
	};
};
