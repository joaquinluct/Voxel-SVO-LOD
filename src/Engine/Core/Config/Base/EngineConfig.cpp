#include "EngineConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(EngineConfig,"EngineConfig");

EngineConfig::EngineConfig(){
	managers_init_order = {
		"DeviceManager",
		"RenderTargetManager",
		"RenderManager",
		"KeyboardManager",
		"CameraManager",
		"AssetManager",
		"ShaderManager",
		"UIManager",
		"GameManager"
	};
	managers_render_order = {
		"RenderTargetManager",
		"GameManager",
		"UIManager",
		"DeviceManager"
	};
	managers_update_order = {
		"KeyboardManager",
		"CameraManager",
		"GameManager",
		"UIManager"
	};
};
