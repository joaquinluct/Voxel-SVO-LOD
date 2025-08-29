#include "EngineConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(EngineConfig,"EngineConfig");

EngineConfig::EngineConfig(){
	managers_init_order = {
		"DeviceManager",
		"InputManager",
		"RenderManager",
		"UpdateManager",
		"KeyboardManager",
		"CameraManager",
		"AssetManager",
		"ShaderManager",
		"UIManager",
		"MouseManager",
		"SceneManager"
	};
	managers_post_init = {
		"CameraManager",
		"RenderManager"
	};
	managers_render_order = {
		"UIManager",
		"DeviceManager"
	};
	managers_update_order = {
		"KeyboardManager",
		"CameraManager",
		"UIManager"
	};
	pipeline_states_init = {
		"SwapChainMain",
		"RasterizedMainColorPass",
		"RasterizedShadowPass",
		"BlendingMainColor",
		"StencilMainColor",
		"ViewportMain",
		"ShaderViewShadow"
	};
};
