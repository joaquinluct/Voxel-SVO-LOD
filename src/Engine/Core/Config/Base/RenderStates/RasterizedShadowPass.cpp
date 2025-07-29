#include "RasterizedShadowPass.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(RasterizedShadowPass,"RasterizedShadowPass");

RasterizedShadowPass::RasterizedShadowPass(){
	name = "RasterizedShadowPass";
	FillMode = 3;
	CullMode = 2;
	FrontCounterClockwise = false;
	DepthBias = 50;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
	AntialiasedLineEnable = false;
	DepthClipEnable = true;
	ScissorEnable = false;
	MultisampleEnable = false;
};
