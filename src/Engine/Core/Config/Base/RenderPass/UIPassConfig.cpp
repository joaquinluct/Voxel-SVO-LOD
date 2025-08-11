#include "UIPassConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(UIPassConfig,"UIPassConfig");

UIPassConfig::UIPassConfig(){
	id = 8;
	enabled = true;
	shader_default = "ShaderTextUI";
	rasterizedState = "RasterizedUIPass";
	viewPortState = "ViewportUI";
	stencilState = "none";
	stencilDef = "StencilStateUI";
	shaderViewState = "none";
	shadowTextureView = "none";
	blendState = "BlendingUI";
	primitiveTopology = 4;
};
