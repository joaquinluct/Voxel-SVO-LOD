#include "GameEngineConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(GameEngineConfig,"GameEngineConfig");

GameEngineConfig::GameEngineConfig(){
	services_init_order = {
		"Chronos",
		"Lighting",
		"Shadows",
		"Skybox",
		"World",
		"Cycle",
		"Terrain",
		"Water",
		"Biome"
	};
	services_post_init = {
		"World",
		"Water"
	};
	services_render_order = {
		"Skybox",
		"Terrain",
		"Water",
		"Lighting"
	};
	service_update_order = {
		"Chronos",
		"Biome",
		"Skybox",
		"Lighting",
		"World",
		"Water",
		"Cycle"
	};
};
