#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class GameEngineConfig : public ConfigService {
public:
	GameEngineConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "GameEngineConfig";
		return className;
	}
	std::vector<std::string> services_init_order;
	std::vector<std::string> services_post_init;
	std::vector<std::string> services_render_order;
	std::vector<std::string> service_update_order;
};
