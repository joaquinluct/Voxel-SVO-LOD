#pragma once

#include <string>
#include <vector>
#include <map>
#include <PassConfigBase.h>

class MainColorPassConfig : public PassConfigBase {
public:
	MainColorPassConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "MainColorPassConfig";
		return className;
	}
	int id;
	bool enabled;
	std::string shader_default;
};
