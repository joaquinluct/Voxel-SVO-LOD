#include "UIDebugConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(UIDebugConfig,"UIDebugConfig");

UIDebugConfig::UIDebugConfig(){
	enabled = false;
	texture = "TextureTextUI";
	shader = "ShaderTextUI";
	text = "Debug Information";
	dependencies = {
		"UIManager"
	};
};
