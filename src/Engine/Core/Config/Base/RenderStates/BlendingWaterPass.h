#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineBlendingState.h>

class BlendingWaterPass : public IPipelineBlendingState {
public:
	BlendingWaterPass();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "BlendingWaterPass";
		return className;
	}
};
