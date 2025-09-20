#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class RendererConfig : public ConfigService {
public:
	RendererConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "RendererConfig";
		return className;
	}
	std::string name;
	std::string version;
};
