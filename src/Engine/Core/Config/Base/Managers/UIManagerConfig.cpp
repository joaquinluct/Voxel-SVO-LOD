#include "UIManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(UIManagerConfig,"UIManagerConfig");

UIManagerConfig::UIManagerConfig(){
	initial_screen = "main_menu.yaml";
	textMesh = "UITextMesh";
	font_path = "Assets/Fonts/Roboto-Regular.ttf";
	font_size = 24;
	dependencies = {
		"DeviceManager",
		"ShaderManager"
	};
};
