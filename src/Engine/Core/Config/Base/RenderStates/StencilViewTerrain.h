#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineStencilView.h>

class StencilViewTerrain : public IPipelineStencilView {
public:
	StencilViewTerrain();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "StencilViewTerrain";
		return className;
	}
	std::string Type;
};
