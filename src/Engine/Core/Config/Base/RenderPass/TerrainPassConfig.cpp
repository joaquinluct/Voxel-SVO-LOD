#include "TerrainPassConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TerrainPassConfig,"TerrainPassConfig");

TerrainPassConfig::TerrainPassConfig(){
	id = 32;
	enabled = true;
	shader_default = "TerrainShader";
	rasterizedState = "RasterizedMainColorPass";
	viewPortState = "ViewportMainColor";
	stencilState = "StencilViewMainColor";
	stencilDef = "none";
	shaderViewState = "none";
	shadowTextureView = "StencilViewShadow";
	blendState = "none";
	primitiveTopology = 4;
};
