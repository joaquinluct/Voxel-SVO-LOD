#pragma once

#include <ConfigBase.h>

class ConfigService: public ConfigBase {
public:
	bool enabled;
	std::string texture;
	std::string shader;
	std::string text;
	std::string mesh;
};
