#include "ReflectionConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ReflectionConfig,"ReflectionConfig");

ReflectionConfig::ReflectionConfig(){
	id = 128;
	enabled = true;
	shader_default = "WaterReflectionShader";
	rasterizedState = "RasterizedReflectedPass";
	viewPortState = "ViewportMainColor";
	stencilState = "StencilViewMainColor";
	stencilDef = "none";
	shaderViewState = "none";
	shadowTextureView = "none";
	blendState = "DisabledBlending";
	primitiveTopology = 4;
};
