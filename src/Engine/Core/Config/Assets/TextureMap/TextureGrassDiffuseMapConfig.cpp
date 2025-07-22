#include "TextureGrassDiffuseMapConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureGrassDiffuseMapConfig,"TextureGrassDiffuseMapConfig");

TextureGrassDiffuseMapConfig::TextureGrassDiffuseMapConfig(){
	texture_type = "JPG";
	albedo = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Rock\\Coast_sand_rock\\coast_sand_rocks_02_diff_4k.jpg";
	normal = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Rock\\Coast_sand_rock\\coast_sand_rocks_02_nor_dx_4k.jpg";
	roughness = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Rock\\Coast_sand_rock\\coast_sand_rocks_02_rough_4k.jpg";
	metallic = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Rock\\Coast_sand_rock\\coast_sand_rocks_02_arm_4k.jpg";
	ao = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Rock\\Coast_sand_rock\\coast_sand_rocks_02_ao_4k.jpg";
};
