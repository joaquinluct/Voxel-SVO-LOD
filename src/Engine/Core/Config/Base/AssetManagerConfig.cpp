#include "AssetManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(AssetManagerConfig,"AssetManagerConfig");

AssetManagerConfig::AssetManagerConfig(){
	manifest_path = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Resources\\assets_manifest.yaml";
	asset_cache_size = 1000;
	asset_load_timeout = 5.0f;
	dependencies = {
		"DeviceManager"
	};
};
