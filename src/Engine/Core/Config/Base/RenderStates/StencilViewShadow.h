#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineStencilView.h>

class StencilViewShadow : public IPipelineStencilView {
public:
	StencilViewShadow();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "StencilViewShadow";
		return className;
	}
	std::string Type;
};
