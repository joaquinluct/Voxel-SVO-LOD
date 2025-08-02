#include "StencilShadow.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(StencilShadow,"StencilShadow");

StencilShadow::StencilShadow(){
	MipLevels = 1;
	ArraySize = 1;
	Format = 47;
	SampleCount = 1;
	SampleQuality = 0;
	Usage = 0;
	BindFlags = 96;
	Depth = 1.0f;
	Stencil = 0.0f;
};
