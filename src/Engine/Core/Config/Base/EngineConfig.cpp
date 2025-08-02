#include "EngineConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(EngineConfig,"EngineConfig");

EngineConfig::EngineConfig(){
	managers_init_order = {
		"DeviceManager",
		"RenderTargetManager",
		"InitManager",
		"RenderManager",
		"KeyboardManager",
		"CameraManager",
		"AssetManager",
		"ShaderManager",
		"GameManager"
	};
	managers_render_order = {
		"RenderTargetManager",
		"GameManager",
		"DeviceManager"
	};
	managers_update_order = {
		"KeyboardManager",
		"CameraManager",
		"GameManager"
	};
	pipeline_states_init = {
		"SwapChainMain",
		"RasterizedMainColorPass",
		"RasterizedShadowPass",
		"BlendingMainColor",
		"StencilMainColor",
		"ViewportMain"
	};
};
