#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineRasterizedState.h>

class RasterizedShadowPass : public IPipelineRasterizedState {
public:
	RasterizedShadowPass();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "RasterizedShadowPass";
		return className;
	}
	std::string name;
};
