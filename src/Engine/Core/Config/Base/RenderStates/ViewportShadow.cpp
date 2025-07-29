#include "ViewportShadow.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(ViewportShadow,"ViewportShadow");

ViewportShadow::ViewportShadow(){
	MinDepth = 0.0f;
	MaxDepth = 1.0f;
	TopLeftX = 0.0f;
	TopLeftY = 0.0f;
	Width = 2048.0f;
	Height = 2048.0f;
};
