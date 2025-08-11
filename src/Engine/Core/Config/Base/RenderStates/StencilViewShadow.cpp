#include "StencilViewShadow.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(StencilViewShadow,"StencilViewShadow");

StencilViewShadow::StencilViewShadow(){
	Type = "D3D11_DEPTH_STENCIL_VIEW_DESC";
	StencilEnable = true;
	DepthEnable = true;
	DepthWriteMask = 1;
	MipLevels = 1;
	ArraySize = 1;
	Format = 44;
	SampleCount = 1;
	SampleQuality = 0;
	Usage = 0;
	BindFlags = 72;
	Depth = 1.0f;
	Stencil = 0.0f;
	Width = 2048.0f;
	Height = 2048.0f;
	ViewDesc = true;
	ViewFormat = 45;
	ViewDimension = 3;
	ViewMipSlice = 0;
	ShaderView = "ShaderViewShadow";
};
