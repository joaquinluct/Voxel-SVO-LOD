#include "Texture2D_WaterReflection.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(Texture2D_WaterReflection,"Texture2D_WaterReflection");

Texture2D_WaterReflection::Texture2D_WaterReflection(){
	MipLevels = 1;
	ArraySize = 1;
	Format = 28;
	SampleDescCount = 1;
	SampleDescQuality = 0;
	Usage = 0;
	BindFlags = 104;
	CPUAccessFlags = 0;
	MiscFlags = 0;
};
