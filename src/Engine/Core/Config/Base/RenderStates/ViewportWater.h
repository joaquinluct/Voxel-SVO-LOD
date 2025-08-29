#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineViewportState.h>

class ViewportWater : public IPipelineViewportState {
public:
	ViewportWater();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "ViewportWater";
		return className;
	}
};
