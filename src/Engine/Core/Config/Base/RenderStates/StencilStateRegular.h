#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineStencilState.h>

class StencilStateRegular : public IPipelineStencilState {
public:
	StencilStateRegular();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "StencilStateRegular";
		return className;
	}
	std::string Type;
};
