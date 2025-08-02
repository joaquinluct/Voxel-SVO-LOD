#include "GameEngineConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(GameEngineConfig,"GameEngineConfig");

GameEngineConfig::GameEngineConfig(){
	services_init_order = {
		"Lighting",
		"Shadows",
		"TestingFloor"
	};
	services_render_order = {
		"TestingFloor",
		"Lighting"
	};
};
