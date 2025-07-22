#include "MeshAssetConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(MeshAssetConfig,"MeshAssetConfig");

MeshAssetConfig::MeshAssetConfig(){
	dependencies = {
		"ShaderManager",
		"DeviceManager",
		"CameraManager",
		"WorldMatrixManager"
	};
};
