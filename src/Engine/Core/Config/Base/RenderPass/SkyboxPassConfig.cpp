#include "SkyboxPassConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(SkyboxPassConfig,"SkyboxPassConfig");

SkyboxPassConfig::SkyboxPassConfig(){
	id = 16;
	enabled = true;
	shader_default = "PBRShadowShader";
	rasterizedState = "RasterizedMainColorPass";
	viewPortState = "ViewportMainColor";
	stencilState = "StencilViewMainColor";
	stencilDef = "none";
	shaderViewState = "none";
	shadowTextureView = "StencilViewShadow";
	blendState = "none";
	primitiveTopology = 4;
};
