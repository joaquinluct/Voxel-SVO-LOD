#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineStencilView.h>

class StencilViewMainColor : public IPipelineStencilView {
public:
	StencilViewMainColor();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "StencilViewMainColor";
		return className;
	}
	std::string Type;
};
