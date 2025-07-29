#include "ShadowPassConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ShadowPassConfig,"ShadowPassConfig");

ShadowPassConfig::ShadowPassConfig(){
	enabled = true;
	shader_name = "ShadowMapShader";
	rasterizedState = "RasterizedShadowPass";
	viewPortState = "ViewportShadow";
	stencilState = "none";
	blendState = "none";
	drawType = "DrawIndexed";
	primitiveTopology = 4;
	resolution = 2048;
};
