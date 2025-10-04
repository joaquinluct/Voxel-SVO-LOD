#include "DebugWindowConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(DebugWindowConfig,"DebugWindowConfig");

DebugWindowConfig::DebugWindowConfig(){
	type = "MainWindowConfig";
	title = "DirectX 12 Voxel Engine - Debug";
	width = 1280;
	height = 720;
	fullscreen = false;
	resizable = true;
	vsync = false;
	show_cursor = true;
	allow_alt_f4 = true;
	show_in_taskbar = true;
	max_delta_time = 0.1f;
	min_delta_time = 0.016f;
	target_fps = 144;
	debug_window = true;
	show_fps = true;
	enable_debug_layer = true;
	capture_mouse = false;
	raw_input = true;
	dependencies = {
		"None"
	};
};
