#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class PipelineConfig : public ConfigBase {
public:
	PipelineConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "PipelineConfig";
		return className;
	}
	std::string mainViewport;
	std::string mainRenderTargetView;
	std::string mainStencilView;
};
