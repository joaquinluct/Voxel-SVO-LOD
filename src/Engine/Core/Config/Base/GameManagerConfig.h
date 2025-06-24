// Configuración de GameManager
#pragma once

#include <string>
#include <vector>

namespace GameManagerConfig{

	struct Values {
		std::string manifestPath = "Resources\\game_manifest.yaml";
		std::string playerEntitConfig = "Data/Entities/player_character_default.yaml";
		std::vector<float> initialPlayerPosition {
			0.000000f,
			100.000000f,
			0.000000f,
		};
		float gameSpeedMultiplier = 1.000000f;
		bool debugMode = true;
		std::vector<std::string> subsystems {
			"SkyboxManager",
		};
	};
};
