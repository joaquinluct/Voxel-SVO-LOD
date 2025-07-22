#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class AssetManagerConfig : public ConfigBase {
public:
	AssetManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "AssetManagerConfig";
		return className;
	}
	std::string manifest_path;
	int asset_cache_size;
	float asset_load_timeout;
};
