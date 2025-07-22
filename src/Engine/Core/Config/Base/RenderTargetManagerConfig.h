#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class RenderTargetManagerConfig : public ConfigBase {
public:
	RenderTargetManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "RenderTargetManagerConfig";
		return className;
	}
	std::vector<float> clear_color;
};
