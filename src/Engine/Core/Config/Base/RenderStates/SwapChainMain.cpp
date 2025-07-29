#include "SwapChainMain.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(SwapChainMain,"SwapChainMain");

SwapChainMain::SwapChainMain(){
	BufferCount = 2;
	Width = 0;
	Height = 0;
	Format = 28;
	Numerator = 60;
	Denominator = 1;
	BufferUsage = "0x00000020UL";
	SampleCount = 1;
	SampleQuality = 0;
	Windowed = true;
	SwapEffect = 4;
};
