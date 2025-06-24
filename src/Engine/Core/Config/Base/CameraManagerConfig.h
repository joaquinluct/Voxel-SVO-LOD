// Configuración de CameraManager
#pragma once

#include <string>
#include <vector>

namespace CameraManagerConfig{

	struct Values {
		std::string initialCamera = "MainCam";
		std::vector<std::string> cameras {
			"MainCam",
			"DebugCam",
		};
		std::vector<std::string> camerasTypes {
			"FirstPersonCamera",
			"FirstPersonCamera",
		};
	};
};
