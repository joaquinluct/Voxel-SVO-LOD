#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineViewportState.h>

class ViewportMainColor : public IPipelineViewportState {
public:
	ViewportMainColor();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "ViewportMainColor";
		return className;
	}
};
