#include "ShaderViewTexture2D.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(ShaderViewTexture2D,"ShaderViewTexture2D");

ShaderViewTexture2D::ShaderViewTexture2D(){
	Type = "D3D11_SHADER_RESOURCE_VIEW_DESC";
	Format = 46;
	ViewDimension = 4;
	MipLevels = 1;
	MostDetailedMip = 0;
};
