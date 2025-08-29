#include "RefractionConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(RefractionConfig,"RefractionConfig");

RefractionConfig::RefractionConfig(){
	id = 256;
	enabled = true;
	shader_default = "WaterRefractionShader";
	rasterizedState = "RasterizedMainColorPass";
	viewPortState = "ViewportMainColor";
	stencilState = "StencilViewMainColor";
	stencilDef = "none";
	shaderViewState = "none";
	shadowTextureView = "none";
	blendState = "DisabledBlending";
	primitiveTopology = 4;
};
