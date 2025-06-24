// Configuración de Physics
#pragma once

#include <string>
#include <vector>

namespace PhysicsConfig{

	struct Values {
		std::string name = "PhysicManager";
		bool active = true;
		std::string version = "1.0.0";
		std::string type = "BasicPhysics";
		std::vector<float> gravity {
			0.000000f,
			-9.810000f,
			0.000000f,
		};
	};
};
