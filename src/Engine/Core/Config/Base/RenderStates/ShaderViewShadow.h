#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineShaderViewState.h>

class ShaderViewShadow : public IPipelineShaderViewState {
public:
	ShaderViewShadow();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "ShaderViewShadow";
		return className;
	}
	std::string Type;
};
