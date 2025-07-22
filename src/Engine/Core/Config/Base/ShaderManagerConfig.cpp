#include "ShaderManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ShaderManagerConfig,"ShaderManagerConfig");

ShaderManagerConfig::ShaderManagerConfig(){
	enabled = true;
	shader_cache_enabled = true;
	shader_paths = {
		"Shaders/Core/",
		"Shaders/Materials/"
	};
	dependencies = {
		"DeviceManager",
		"ShaderAsset",
		"AssetManager"
	};
};
