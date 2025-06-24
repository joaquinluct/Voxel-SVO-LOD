// Configuración de MainCam
#pragma once

#include <string>
#include <vector>

namespace MainCamConfig{

	struct Values {
		std::string type = "FirstPersonCamera";
		bool active = true;
		float move_speed = 150.000000f;
		float rotation_speed = 120.000000f;
		float fov_y = 60.000000f;
		float near_clip = 0.100000f;
		float far_clip = 6000.000000f;
		std::vector<float> position {
			0.000000f,
			100.000000f,
			0.000000f,
		};
		float initial_pitch = 0.000000f;
		float initial_yaw = 0.000000f;
	};
};
