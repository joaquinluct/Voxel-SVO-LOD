#include "StencilStateRegular.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(StencilStateRegular,"StencilStateRegular");

StencilStateRegular::StencilStateRegular(){
	Type = "D3D11_DEPTH_STENCIL_DESC";
	DepthEnable = 1;
	DepthWriteMask = 1;
	DepthFunc = 2;
	StencilEnable = 0;
	StencilReadMask = 255;
};
