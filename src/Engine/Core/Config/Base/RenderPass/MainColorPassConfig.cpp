#include "MainColorPassConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(MainColorPassConfig,"MainColorPassConfig");

MainColorPassConfig::MainColorPassConfig(){
	id = 2;
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
