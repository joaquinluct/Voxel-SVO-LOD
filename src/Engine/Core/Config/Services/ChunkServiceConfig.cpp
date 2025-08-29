#include "ChunkServiceConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ChunkServiceConfig,"ChunkServiceConfig");

ChunkServiceConfig::ChunkServiceConfig(){
	enabled = true;
	dependencies = {
		"CameraManager"
	};
};
