#include "TerrainAssetConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TerrainAssetConfig,"TerrainAssetConfig");

TerrainAssetConfig::TerrainAssetConfig(){
	dependencies = {
		"ShaderManager",
		"DeviceManager",
		"CameraManager"
	};
};
