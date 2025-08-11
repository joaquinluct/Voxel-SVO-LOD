#include "StencilViewMainColor.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(StencilViewMainColor,"StencilViewMainColor");

StencilViewMainColor::StencilViewMainColor(){
	Type = "D3D11_DEPTH_STENCIL_VIEW_DESC";
	StencilEnable = true;
	DepthEnable = true;
	DepthWriteMask = 1;
	MipLevels = 1;
	ArraySize = 1;
	Format = 45;
	SampleCount = 1;
	SampleQuality = 0;
	Usage = 0;
	BindFlags = 64;
	Depth = 1.0f;
	Stencil = 0.0f;
	ViewDesc = false;
};
