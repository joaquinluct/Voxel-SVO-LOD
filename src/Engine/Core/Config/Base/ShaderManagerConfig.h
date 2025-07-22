#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class ShaderManagerConfig : public ConfigBase {
public:
	ShaderManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ShaderManagerConfig";
		return className;
	}
	bool enabled;
	bool shader_cache_enabled;
	std::vector<std::string> shader_paths;
};
