#include "Texture2D_Simple_Jpg.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(Texture2D_Simple_Jpg,"Texture2D_Simple_Jpg");

Texture2D_Simple_Jpg::Texture2D_Simple_Jpg(){
	MipLevels = 1;
	ArraySize = 1;
	Format = 28;
	SampleDescCount = 1;
	SampleDescQuality = 0;
	Usage = 1;
	BindFlags = 8;
	CPUAccessFlags = 0;
	MiscFlags = 0;
};
