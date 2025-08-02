#include "StencilMainColor.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(StencilMainColor,"StencilMainColor");

StencilMainColor::StencilMainColor(){
	MipLevels = 1;
	ArraySize = 1;
	Format = 45;
	SampleCount = 1;
	SampleQuality = 0;
	Usage = 0;
	BindFlags = 64;
	Depth = 1.0f;
	Stencil = 0.0f;
};
