#include "BlendingMainColor.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(BlendingMainColor,"BlendingMainColor");

BlendingMainColor::BlendingMainColor(){
	BlendEnable = true;
	SrcBlend = 5;
	DestBlend = 6;
	BlendOp = 1;
	SrcBlendAlpha = 2;
	DestBlendAlpha = 1;
	BlendOpAlpha = 1;
	RenderTargetWriteMask = 15;
};
