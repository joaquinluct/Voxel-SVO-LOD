#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineShaderViewState.h>

class ShaderViewTexture2D : public IPipelineShaderViewState {
public:
	ShaderViewTexture2D();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "ShaderViewTexture2D";
		return className;
	}
	std::string Type;
};
