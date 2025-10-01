#include "UIAssetConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(UIAssetConfig,"UIAssetConfig");

UIAssetConfig::UIAssetConfig(){
	dependencies = {
		"ShaderManager",
		"DeviceManager",
		"CameraManager"
	};
};
