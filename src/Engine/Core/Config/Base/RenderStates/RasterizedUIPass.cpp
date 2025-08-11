#include "RasterizedUIPass.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(RasterizedUIPass,"RasterizedUIPass");

RasterizedUIPass::RasterizedUIPass(){
	name = "RasterizedUIPass";
	FillMode = 3;
	CullMode = 1;
	FrontCounterClockwise = false;
	DepthBias = 0;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 0.0f;
	AntialiasedLineEnable = false;
	DepthClipEnable = false;
	ScissorEnable = false;
	MultisampleEnable = false;
};
