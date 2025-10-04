#include "KeyboardManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(KeyboardManagerConfig,"KeyboardManagerConfig");

KeyboardManagerConfig::KeyboardManagerConfig(){
	service = "Keyboard";
	key_bindings = {
			{"move_forward", {"KEY_W","KEY_UP"}},
			{"move_backward", {"KEY_S","KEY_DOWN"}},
			{"move_left", {"KEY_A","KEY_LEFT"}},
			{"move_right", {"KEY_D","KEY_RIGHT"}},
			{"jump", {"KEY_SPACE","MOUSE_BUTTON3"}},
			{"crouch", {"KEY_CTRL_LEFT","MOUSE_BUTTON2"}},
			{"sprint", {"KEY_SHIFT_LEFT","KEY_SHIFT_RIGHT","KEY_CTRL_LEFT","KEY_CTRL_RIGHT"}},
			{"interact", {"KEY_E","MOUSE_BUTTON1"}},
			{"inventory", {"KEY_I","KEY_TAB"}},
			{"switch_camera", {"KEY_C"}},
	};
	dependencies = {
		"Keybindings",
		"Keyboard"
	};
};
