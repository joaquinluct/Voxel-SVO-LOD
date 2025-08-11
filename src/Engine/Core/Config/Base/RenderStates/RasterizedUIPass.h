#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineRasterizedState.h>

class RasterizedUIPass : public IPipelineRasterizedState {
public:
	RasterizedUIPass();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "RasterizedUIPass";
		return className;
	}
	std::string name;
};
