#include "ShaderAssetConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ShaderAssetConfig,"ShaderAssetConfig");

ShaderAssetConfig::ShaderAssetConfig(){
	dependencies = {
		"DeviceManager"
	};
};
