#include "BasePassConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(BasePassConfig,"BasePassConfig");

BasePassConfig::BasePassConfig(){
	id = 0;
	enabled = true;
	SwapChain = "SwapChainMain";
	BackBuffer = "BackBufferMain";
	DepthBuffer = "DepthBufferMain";
	DepthStencil = "DepthStencilMain";
	RenderTarget = "MainRenderTarget";
	StencilView = "StencilViewMainColor";
	viewPortState = "ViewportMainColor";
	rasterizedState = "RasterizedMainColorPass";
	passes = {
		"ShadowPass",
		"MainColorPass",
		"UIPass",
		"SkyboxPass",
		"TerrainPass",
		"LinePass",
		"Reflection",
		"Refraction",
		"WaterPass"
	};
};
