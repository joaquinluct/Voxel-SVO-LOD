#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineViewportState.h>

class ViewportUI : public IPipelineViewportState {
public:
	ViewportUI();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "ViewportUI";
		return className;
	}
};
