#include "BlendingWaterPass.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(BlendingWaterPass,"BlendingWaterPass");

BlendingWaterPass::BlendingWaterPass(){
	AlphaToCoverageEnable = false;
	IndependentBlendEnable = false;
	BlendEnable = true;
	SrcBlend = 5;
	DestBlend = 6;
	BlendOp = 1;
	SrcBlendAlpha = 5;
	DestBlendAlpha = 6;
	BlendOpAlpha = 1;
	RenderTargetWriteMask = 15;
};
