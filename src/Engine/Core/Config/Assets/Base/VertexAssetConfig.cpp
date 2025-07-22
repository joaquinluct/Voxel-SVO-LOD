#include "VertexAssetConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(VertexAssetConfig,"VertexAssetConfig");

VertexAssetConfig::VertexAssetConfig(){
	dependencies = {
		"DeviceManager"
	};
};
