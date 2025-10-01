#pragma once

#include <string>
#include <vector>
#include <map>
#include <IBackBufferState.h>

class BackBufferMain : public IBackBufferState {
public:
	BackBufferMain();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "BackBufferMain";
		return className;
	}
	std::string Type;
};
