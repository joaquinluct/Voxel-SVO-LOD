#include "ShadowPassConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ShadowPassConfig,"ShadowPassConfig");

ShadowPassConfig::ShadowPassConfig(){
	id = 1;
	enabled = true;
	shader_name = "ShadowMapShader";
	rasterizedState = "RasterizedShadowPass";
	viewPortState = "ViewportShadow";
	stencilState = "StencilViewShadow";
	stencilDef = "StencilStateRegular";
	shaderViewState = "ShaderViewShadow";
	shadowTextureView = "none";
	blendState = "DisabledBlending";
	primitiveTopology = 4;
	resolution = 2048;
};
