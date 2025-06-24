// Configuración de debugCam
#pragma once

#include <string>
#include <vector>

namespace debugCamConfig{

	struct Values {
		std::string type = "FirstPersonCamera";
		bool active = false;
		float move_speed = 500.000000f;
		float rotation_speed = 180.000000f;
		float fov_y = 75.000000f;
		float near_clip = 0.100000f;
		float far_clip = 10000.000000f;
		std::vector<float> position {
			500.000000f,
			1500.000000f,
			500.000000f,
		};
	};
};
