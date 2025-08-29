#include "LinePassConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(LinePassConfig,"LinePassConfig");

LinePassConfig::LinePassConfig(){
	id = 64;
	enabled = true;
	shader_default = "BaseShader";
	rasterizedState = "RasterizedMainColorPass";
	viewPortState = "ViewportMainColor";
	stencilState = "StencilViewMainColor";
	stencilDef = "none";
	shaderViewState = "none";
	shadowTextureView = "none";
	blendState = "none";
	primitiveTopology = 4;
};
