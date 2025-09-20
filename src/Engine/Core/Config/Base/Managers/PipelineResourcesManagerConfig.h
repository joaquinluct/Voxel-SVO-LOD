#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class PipelineResourcesManagerConfig : public ConfigBase {
public:
	PipelineResourcesManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "PipelineResourcesManagerConfig";
		return className;
	}
};
