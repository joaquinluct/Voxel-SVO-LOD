#include "TextureAssetConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureAssetConfig,"TextureAssetConfig");

TextureAssetConfig::TextureAssetConfig(){
	dependencies = {
		"DeviceManager"
	};
};
