#include "MainWindowConfig.h"

MainWindowConfig::MainWindowConfig(){
	type = "MainWindowConfig";
	title = "DirectX 12 Voxel Engine";
	width = 1920;
	height = 1080;
	fullscreen = false;
	resizable = true;
	vsync = true;
	show_cursor = true;
	allow_alt_f4 = true;
	show_in_taskbar = true;
	max_delta_time = 0.052f;
	min_delta_time = 0.042f;
	target_fps = 60;
	debug_window = true;
	show_fps = false;
	enable_debug_layer = true;
	capture_mouse = false;
	raw_input = false;
	dependencies = {
		"None"
	};
}
