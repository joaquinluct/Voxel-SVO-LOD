#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineBlendingState.h>

class DisabledBlending : public IPipelineBlendingState {
public:
	DisabledBlending();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "DisabledBlending";
		return className;
	}
	std::string TYPE;
};
