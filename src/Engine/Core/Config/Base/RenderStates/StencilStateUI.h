#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineStencilState.h>

class StencilStateUI : public IPipelineStencilState {
public:
	StencilStateUI();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "StencilStateUI";
		return className;
	}
	std::string Type;
	int StencilWriteMask;
};
