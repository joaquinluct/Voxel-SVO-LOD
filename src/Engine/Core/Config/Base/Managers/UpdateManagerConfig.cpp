#include "UpdateManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(UpdateManagerConfig,"UpdateManagerConfig");

UpdateManagerConfig::UpdateManagerConfig(){
	dependencies = {
		"SceneManager",
		"CameraManager",
		"World"
	};
};
