#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineBlendingState.h>

class BlendingUI : public IPipelineBlendingState {
public:
	BlendingUI();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "BlendingUI";
		return className;
	}
};
