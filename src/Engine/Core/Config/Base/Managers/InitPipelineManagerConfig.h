#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class InitPipelineManagerConfig : public ConfigBase {
public:
	InitPipelineManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "InitPipelineManagerConfig";
		return className;
	}
	std::string deviceAndSwapChain;
	std::vector<std::string> rasterizedStates;
	std::vector<std::string> blendingStates;
	std::vector<std::string> stencilViews;
	std::vector<std::string> stencilStates;
	std::vector<std::string> viewPortStates;
};
