#include "SkyboxPassConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(SkyboxPassConfig,"SkyboxPassConfig");

SkyboxPassConfig::SkyboxPassConfig(){
	id = 16;
	enabled = true;
	shader_default = "PBRShadowShader";
	rasterizedState = "RasterizedMainColorPass";
	viewPortState = "ViewportMainColor";
	stencilState = "none";
	stencilDef = "none";
	shaderViewState = "none";
	shadowTextureView = "none";
	blendState = "none";
	primitiveTopology = 4;
};
