#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineSwapChainState.h>

class SwapChainMain : public IPipelineSwapChainState {
public:
	SwapChainMain();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "SwapChainMain";
		return className;
	}
};
