#include "WaterPassConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(WaterPassConfig,"WaterPassConfig");

WaterPassConfig::WaterPassConfig(){
	id = 512;
	enabled = true;
	shader_default = "OceanShader";
	rasterizedState = "RasterizedMainColorPass";
	viewPortState = "ViewportMainColor";
	stencilState = "StencilViewMainColor";
	stencilDef = "none";
	shaderViewState = "none";
	shadowTextureView = "StencilViewShadow";
	blendState = "BlendingWaterPass";
	primitiveTopology = 4;
};
