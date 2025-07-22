#include "UIBaseConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(UIBaseConfig,"UIBaseConfig");

UIBaseConfig::UIBaseConfig(){
	enabled = true;
	texture = "TextureUI";
	shader = "ShaderUI";
	text = "User Interface";
	dependencies = {
		"RenderTargetManager",
		"DeviceManager",
		"UIManager"
	};
};
