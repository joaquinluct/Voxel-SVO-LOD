#include "CameraManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(CameraManagerConfig,"CameraManagerConfig");

CameraManagerConfig::CameraManagerConfig(){
	initialCamera = "MainCam";
	cameras = {
		"MainCam",
		"DebugCam"
	};
	camerasTypes = {
		"FirstPersonCamera"
	};
	dependencies = {
		"Mouse",
		"Keyboard",
		"KeyboardManager",
		"RenderTargetManager"
	};
};
