#include "Texture2D_Array_Jpg.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(Texture2D_Array_Jpg,"Texture2D_Array_Jpg");

Texture2D_Array_Jpg::Texture2D_Array_Jpg(){
	MipLevels = 0;
	Format = 28;
	SampleDescCount = 1;
	SampleDescQuality = 0;
	Usage = 0;
	BindFlags = 40;
	CPUAccessFlags = 0;
	MiscFlags = 1;
};
