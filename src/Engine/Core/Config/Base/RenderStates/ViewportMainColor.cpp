#include "ViewportMainColor.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(ViewportMainColor,"ViewportMainColor");

ViewportMainColor::ViewportMainColor(){
	MinDepth = 0.0f;
	MaxDepth = 1.0f;
	TopLeftX = 0.0f;
	TopLeftY = 0.0f;
	Width = 0.0f;
	Height = 0.0f;
};
