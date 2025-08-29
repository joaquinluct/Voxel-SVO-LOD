#include "MainIndexConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(MainIndexConfig,"MainIndexConfig");

MainIndexConfig::MainIndexConfig(){
	mainIndex = {
		"ProceduralTerrain",
		"SkyboxShader",
		"ProceduralSkyboxShader",
		"DefaultShader",
		"ShaderTextUI",
		"TextureBasicShader",
		"ShadowMapShader",
		"TextureMapShader",
		"PBRShader",
		"PBRShadowShader",
		"FloorVertex",
		"FloorMesh",
		"House1Mesh",
		"TextureTextUI",
		"TextureGrassDiffuse",
		"TextureBricks001",
		"DefaultSkybox",
		"SkyboxCube",
		"SkyboxMesh",
		"ProcSkyboxMesh",
		"BoxMesh",
		"UITextMesh",
		"TerrainMesh",
		"TerrainShader",
		"Terrain2Shader",
		"ProceduralTerrainTexture",
		"OceanShader",
		"OceanMesh",
		"BlendingWaterPass"
	};
	post_init = {
		"none"
	};
};
