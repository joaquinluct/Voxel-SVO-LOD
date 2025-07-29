#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class ShadowPassConfig : public ConfigBase {
public:
	ShadowPassConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ShadowPassConfig";
		return className;
	}
	bool enabled;
	std::string shader_name;
	std::string rasterizedState;
	std::string viewPortState;
	std::string stencilState;
	std::string blendState;
	std::string drawType;
	int primitiveTopology;
	int resolution;
};
