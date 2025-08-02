#include "InitManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(InitManagerConfig,"InitManagerConfig");

InitManagerConfig::InitManagerConfig(){
	deviceAndSwapChain = "SwapChainMain";
	rasterizedStates = {
		"RasterizedMainColorPass",
		"RasterizedShadowPass"
	};
	blendingStates = {
		"BlendingMainColor"
	};
	stencilStates = {
		"StencilMainColor",
		"StencilShadow"
	};
	viewPortStates = {
		"ViewportMainColor",
		"ViewportShadow"
	};
	dependencies = {
		"Shadows"
	};
};
