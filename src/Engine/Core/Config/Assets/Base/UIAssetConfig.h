#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class UIAssetConfig : public ConfigBase {
public:
	UIAssetConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "UIAssetConfig";
		return className;
	}
};
