#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineShaderViewState.h>

class ShaderViewReflection : public IPipelineShaderViewState {
public:
	ShaderViewReflection();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "ShaderViewReflection";
		return className;
	}
	std::string Type;
};
