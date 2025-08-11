#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class SkyboxPassConfig : public ConfigBase {
public:
	SkyboxPassConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "SkyboxPassConfig";
		return className;
	}
	int id;
	bool enabled;
	std::string shader_default;
	std::string rasterizedState;
	std::string viewPortState;
	std::string stencilState;
	std::string stencilDef;
	std::string shaderViewState;
	std::string shadowTextureView;
	std::string blendState;
	int primitiveTopology;
};
