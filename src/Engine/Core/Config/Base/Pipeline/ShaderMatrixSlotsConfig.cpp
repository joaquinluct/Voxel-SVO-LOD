#include "ShaderMatrixSlotsConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ShaderMatrixSlotsConfig,"ShaderMatrixSlotsConfig");

ShaderMatrixSlotsConfig::ShaderMatrixSlotsConfig(){
	slots = {
		"MatrixBufferType",
		"CameraData",
		"DirectionalLight",
		"MaterialData",
		"TextureTransformations",
		"LightSpaceMatrices",
		"ProceduralSkyboxMatrix",
		"WaterData",
		"TimeData",
		"WaterInstance",
		"ShadowMapMatrices",
		"TerrainBlendBuffer",
		"Terrain2BlendBuffer",
		"MatrixBufferTypeOrthographic"
	};
};
