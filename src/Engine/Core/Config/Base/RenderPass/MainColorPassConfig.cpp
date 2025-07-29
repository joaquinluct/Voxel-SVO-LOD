#include "MainColorPassConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(MainColorPassConfig,"MainColorPassConfig");

MainColorPassConfig::MainColorPassConfig(){
	enabled = true;
	shader_default = "PBRShadowShader";
	rasterizedState = "RasterizedMainColorPass";
	viewPortState = "ViewportMainColor";
	stencilState = "StencilMainColor";
	blendState = "none";
	drawType = "DrawIndexed";
	primitiveTopology = 4;
};
