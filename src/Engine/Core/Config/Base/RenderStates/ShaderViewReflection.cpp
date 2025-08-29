#include "ShaderViewReflection.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(ShaderViewReflection,"ShaderViewReflection");

ShaderViewReflection::ShaderViewReflection(){
	Type = "D3D11_SHADER_RESOURCE_VIEW_DESC";
	Format = 28;
	ViewDimension = 4;
	MipLevels = 1;
	MostDetailedMip = 0;
};
