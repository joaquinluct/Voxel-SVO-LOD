#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class RenderManagerConfig : public ConfigBase {
public:
	RenderManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "RenderManagerConfig";
		return className;
	}
};
