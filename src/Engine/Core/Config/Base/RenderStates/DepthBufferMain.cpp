#include "DepthBufferMain.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(DepthBufferMain,"DepthBufferMain");

DepthBufferMain::DepthBufferMain(){
	Type = "D3D11_TEXTURE2D_DESC";
	Width = 0;
	Height = 0;
	MipLevels = 1;
	ArraySize = 1;
	Format = 45;
	SampleDescCount = 1;
	SampleDescQuality = 0;
	Usage = 0;
	BindFlags = 64;
	CPUAccessFlags = 0;
	MiscFlags = 0;
};
