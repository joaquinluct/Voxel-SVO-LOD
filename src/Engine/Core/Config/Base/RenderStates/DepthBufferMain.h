#pragma once

#include <string>
#include <vector>
#include <map>
#include <IBackBufferState.h>

class DepthBufferMain : public IBackBufferState {
public:
	DepthBufferMain();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "DepthBufferMain";
		return className;
	}
	std::string Type;
};
