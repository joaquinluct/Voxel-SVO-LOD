#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class MainColorPassConfig : public ConfigBase {
public:
	MainColorPassConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "MainColorPassConfig";
		return className;
	}
	bool enabled;
	std::string shader_default;
	std::string rasterizedState;
	std::string viewPortState;
	std::string stencilState;
	std::string blendState;
	std::string drawType;
	int primitiveTopology;
};
