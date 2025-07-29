#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineBlendingState.h>

class BlendingMainColor : public IPipelineBlendingState {
public:
	BlendingMainColor();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "BlendingMainColor";
		return className;
	}
};
