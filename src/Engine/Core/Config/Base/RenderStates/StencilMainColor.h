#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineStencilState.h>

class StencilMainColor : public IPipelineStencilState {
public:
	StencilMainColor();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "StencilMainColor";
		return className;
	}
};
