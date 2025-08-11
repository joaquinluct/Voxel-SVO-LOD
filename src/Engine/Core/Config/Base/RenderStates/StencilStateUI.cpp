#include "StencilStateUI.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(StencilStateUI,"StencilStateUI");

StencilStateUI::StencilStateUI(){
	Type = "D3D11_DEPTH_STENCIL_DESC";
	DepthEnable = 0;
	DepthWriteMask = 0;
	DepthFunc = 8;
	StencilEnable = 0;
	StencilReadMask = 255;
	StencilWriteMask = 255;
};
