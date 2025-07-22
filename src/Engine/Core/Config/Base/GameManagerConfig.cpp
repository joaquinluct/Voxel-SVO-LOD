#include "GameManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(GameManagerConfig,"GameManagerConfig");

GameManagerConfig::GameManagerConfig(){
	manifestPath = "Resources\\game_manifest.yaml";
	playerEntitConfig = "Data/Entities/player_character_default.yaml";
	initialPlayerPosition = {
		0.0f,
		100.0f,
		0.0f
	};
	gameSpeedMultiplier = 1.0f;
	debugMode = true;
	dependencies = {
		"DeviceManager"
	};
};
