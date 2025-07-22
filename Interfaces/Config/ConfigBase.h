#pragma once

#include <string>
#include <vector>
#include <IConfig.h>

class ConfigBase : public IConfig {
public:
	std::string type;
	std::vector<std::string> dependencies;
};
