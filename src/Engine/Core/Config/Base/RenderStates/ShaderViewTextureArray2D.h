#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineShaderViewState.h>

class ShaderViewTextureArray2D : public IPipelineShaderViewState {
public:
	ShaderViewTextureArray2D();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "ShaderViewTextureArray2D";
		return className;
	}
	std::string Type;
	int Texture2DArrayMostDetailedMip;
	int Texture2DArrayMipLevels;
	int Texture2DArrayFirstArraySlice;
	int Texture2DArrayArraySize;
};
