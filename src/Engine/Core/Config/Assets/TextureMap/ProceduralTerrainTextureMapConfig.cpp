#include "ProceduralTerrainTextureMapConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ProceduralTerrainTextureMapConfig,"ProceduralTerrainTextureMapConfig");

ProceduralTerrainTextureMapConfig::ProceduralTerrainTextureMapConfig(){
	texture_type = "JPG_ARRAY";
	texture2DArray = {
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Grass\\GrassPatchyGround\\Poliigon_GrassPatchyGround_4585_BaseColor.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Dirt\\GroundDirtWeedsPatchy\\GroundDirtWeedsPatchy004_COL_2K.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Rock\\Coast_sand_rock\\coast_sand_rocks_02_diff_2k.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Sand\\GroundSand005\\GroundSand005_COL_2K.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Snow\\IA\\snow_albedo.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Grass\\GrassPatchyGround\\Poliigon_GrassPatchyGround_4585_Normal.png",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Dirt\\GroundDirtWeedsPatchy\\GroundDirtWeedsPatchy004_NRM_2K.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Rock\\Coast_sand_rock\\coast_sand_rocks_02_nor_dx_2k.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Sand\\GroundSand005\\GroundSand005_NRM_2K.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Snow\\IA\\snow_normal.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Grass\\GrassPatchyGround\\Poliigon_GrassPatchyGround_4585_Roughness.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Dirt\\GroundDirtWeedsPatchy\\GroundDirtWeedsPatchy004_GLOSS_2K.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Rock\\Coast_sand_rock\\coast_sand_rocks_02_rough_2k.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Sand\\GroundSand005\\GroundSand005_GLOSS_2K.jpg",
		"C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Textures\\Snow\\IA\\snow_roughness.jpg"
	};
};
