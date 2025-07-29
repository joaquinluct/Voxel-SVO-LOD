#include "WorldMatrixManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(WorldMatrixManagerConfig,"WorldMatrixManagerConfig");

WorldMatrixManagerConfig::WorldMatrixManagerConfig(){
	max_instances_per_batch = 1024;
	dependencies = {
		"DeviceManager",
		"RenderTargetManager",
		"CameraManager5"
	};
};
