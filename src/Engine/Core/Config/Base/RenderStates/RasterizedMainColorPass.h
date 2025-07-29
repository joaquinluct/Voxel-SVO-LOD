#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineRasterizedState.h>

class RasterizedMainColorPass : public IPipelineRasterizedState {
public:
	RasterizedMainColorPass();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "RasterizedMainColorPass";
		return className;
	}
	std::string name;
};
