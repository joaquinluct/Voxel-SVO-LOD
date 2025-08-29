#include "ShaderViewTextureArray2D.h"
#include <REGISTER_PIPELINE_STATE_MACRO.h>

REGISTER_PIPELINE_STATE_TYPE(ShaderViewTextureArray2D,"ShaderViewTextureArray2D");

ShaderViewTextureArray2D::ShaderViewTextureArray2D(){
	Type = "D3D11_SHADER_RESOURCE_VIEW_DESC";
	Format = 28;
	ViewDimension = 5;
	Texture2DArrayMostDetailedMip = 0;
	Texture2DArrayMipLevels = -1;
	Texture2DArrayFirstArraySlice = 0;
	Texture2DArrayArraySize = 12;
};
