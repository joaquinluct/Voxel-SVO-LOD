#pragma once

#include <string>
#include "ConfigBase.h"

class PassConfigBase: public ConfigBase
{
public:
	virtual ~PassConfigBase() = default;
	int id;
	bool enabled;
	std::string shader_name;
	std::string rasterizedState;
	std::string viewPortState;
	std::string stencilState;
	std::string stencilDef;
	std::string shaderViewState;
	bool shadowTextureView;
	std::string blendState;
	int primitiveTopology;
	int resolution;
};
