#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineStencilState.h>

class StencilShadow : public IPipelineStencilState {
public:
	StencilShadow();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "StencilShadow";
		return className;
	}
};
