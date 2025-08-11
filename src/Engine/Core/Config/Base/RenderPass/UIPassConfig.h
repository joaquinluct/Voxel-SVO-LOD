#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class UIPassConfig : public ConfigBase {
public:
	UIPassConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "UIPassConfig";
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
