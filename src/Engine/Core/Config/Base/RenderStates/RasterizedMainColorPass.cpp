#include "RasterizedMainColorPass.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(RasterizedMainColorPass,"RasterizedMainColorPass");

RasterizedMainColorPass::RasterizedMainColorPass(){
	name = "RasterizedMainColorPass";
	FillMode = 3;
	CullMode = 3;
	FrontCounterClockwise = false;
	DepthBias = 0;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 0.0f;
	AntialiasedLineEnable = true;
	DepthClipEnable = true;
	ScissorEnable = true;
	MultisampleEnable = true;
};
