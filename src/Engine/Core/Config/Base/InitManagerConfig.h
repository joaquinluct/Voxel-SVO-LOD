#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class InitManagerConfig : public ConfigBase {
public:
	InitManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "InitManagerConfig";
		return className;
	}
	std::string deviceAndSwapChain;
	std::vector<std::string> rasterizedStates;
	std::vector<std::string> blendingStates;
	std::vector<std::string> stencilStates;
	std::vector<std::string> viewPortStates;
};
