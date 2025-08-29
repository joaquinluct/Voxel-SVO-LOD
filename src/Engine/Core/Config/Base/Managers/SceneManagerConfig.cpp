#include "SceneManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(SceneManagerConfig,"SceneManagerConfig");

SceneManagerConfig::SceneManagerConfig(){
	dependencies = {
		"CameraManager",
		"World"
	};
};
