#include "InitManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(InitManagerConfig,"InitManagerConfig");

InitManagerConfig::InitManagerConfig(){
	deviceAndSwapChain = "SwapChainMain";
	rasterizedStates = {
		"RasterizedMainColorPass",
		"RasterizedShadowPass",
		"RasterizedUIPass"
	};
	blendingStates = {
		"DisabledBlending",
		"BlendingUI"
	};
	stencilViews = {
		"StencilViewMainColor",
		"StencilViewShadow"
	};
	stencilStates = {
		"StencilStateRegular",
		"StencilStateUI"
	};
	viewPortStates = {
		"ViewportMainColor",
		"ViewportShadow",
		"ViewportUI"
	};
	dependencies = {
		"Shadows"
	};
};
