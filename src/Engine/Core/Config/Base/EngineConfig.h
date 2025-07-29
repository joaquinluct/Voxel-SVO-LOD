#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class EngineConfig : public ConfigBase {
public:
	EngineConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "EngineConfig";
		return className;
	}
	std::vector<std::string> managers_init_order;
	std::vector<std::string> managers_render_order;
	std::vector<std::string> managers_update_order;
	std::vector<std::string> pipeline_states_init;
};
