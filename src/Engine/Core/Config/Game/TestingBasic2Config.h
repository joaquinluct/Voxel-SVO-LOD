// Configuración de TestingBasic2
#pragma once

#include <string>
#include <vector>

namespace TestingBasic2Config{

	struct Values {
		std::string name = "TestingBasicManager";
		bool active = true;
		std::string version = "1.0.0";
		std::string type = "TestingBasic2";
		std::string description = "Un tri\xE1ngulo de pruebas b\xE1sicas.";
	};
};
