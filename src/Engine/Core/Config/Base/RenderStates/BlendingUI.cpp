#include "BlendingUI.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(BlendingUI,"BlendingUI");

BlendingUI::BlendingUI(){
	AlphaToCoverageEnable = false;
	IndependentBlendEnable = false;
	BlendEnable = true;
	SrcBlend = 5;
	DestBlend = 6;
	BlendOp = 1;
	SrcBlendAlpha = 2;
	DestBlendAlpha = 1;
	BlendOpAlpha = 1;
	RenderTargetWriteMask = 15;
};
