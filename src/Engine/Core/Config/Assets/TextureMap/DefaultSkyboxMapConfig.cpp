#include "DefaultSkyboxMapConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(DefaultSkyboxMapConfig,"DefaultSkyboxMapConfig");

DefaultSkyboxMapConfig::DefaultSkyboxMapConfig(){
	texture_type = "CUBEMAP";
	albedo = "C:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Skybox\\defaultSkybox.dds";
};
