#include "DisabledBlending.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(DisabledBlending,"DisabledBlending");

DisabledBlending::DisabledBlending(){
	TYPE = "D3D11_BLEND_DESC";
	AlphaToCoverageEnable = false;
	IndependentBlendEnable = false;
	BlendEnable = false;
	SrcBlend = 1;
	DestBlend = 1;
	BlendOp = 1;
	SrcBlendAlpha = 1;
	DestBlendAlpha = 1;
	BlendOpAlpha = 1;
	RenderTargetWriteMask = 15;
};
