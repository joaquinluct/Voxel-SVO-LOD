#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineShaderViewState.h>

class ShaderViewRefraction : public IPipelineShaderViewState {
public:
	ShaderViewRefraction();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "ShaderViewRefraction";
		return className;
	}
	std::string Type;
};
