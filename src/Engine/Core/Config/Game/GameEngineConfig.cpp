#include "GameEngineConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(GameEngineConfig,"GameEngineConfig");

GameEngineConfig::GameEngineConfig(){
	terrain_engine_init_order = {
		"TerrainChunkEngine",
		"GeometryChunkEngine"
	};
	services_init_order = {
		"Chronos",
		"Lighting",
		"Shadows",
		"Skybox",
		"World",
		"Cycle"
	};
	services_post_init = {
		"World",
		"Water"
	};
	services_render_order = {
		"Skybox",
		"Lighting"
	};
	service_update_order = {
		"Chronos",
		"Skybox",
		"Lighting",
		"World",
		"Cycle"
	};
};
