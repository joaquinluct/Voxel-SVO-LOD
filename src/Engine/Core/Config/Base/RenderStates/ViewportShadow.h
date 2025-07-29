#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineViewportState.h>

class ViewportShadow : public IPipelineViewportState {
public:
	ViewportShadow();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "ViewportShadow";
		return className;
	}
};
