#include "TextureBricks001MapConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureBricks001MapConfig,"TextureBricks001MapConfig");

TextureBricks001MapConfig::TextureBricks001MapConfig(){
	texture_type = "JPG";
	albedo = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Brick\\Poliigon_BrickWallReclaimed_8320_BaseColor.jpg";
	normal = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Brick\\Poliigon_BrickWallReclaimed_8320_Normal.jpg";
	roughness = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Brick\\Poliigon_BrickWallReclaimed_8320_Roughness.jpg";
	metallic = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Brick\\Poliigon_BrickWallReclaimed_8320_Metallic.jpg";
	ao = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Brick\\Poliigon_BrickWallReclaimed_8320_AmbientOcclusion.jpg";
	displacement = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Brick\\Poliigon_BrickWallReclaimed_8320_Displacement.tiff";
};
