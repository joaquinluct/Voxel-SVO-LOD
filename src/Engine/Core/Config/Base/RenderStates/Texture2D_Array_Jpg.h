#pragma once

#include <string>
#include <vector>
#include <map>
#include <IPipelineTexture2DDesc.h>

class Texture2D_Array_Jpg : public IPipelineTexture2DDesc {
public:
	Texture2D_Array_Jpg();
	static const std::string& GetStaticPipelineStateName()
	{
		static const std::string className = "Texture2D_Array_Jpg";
		return className;
	}
};
