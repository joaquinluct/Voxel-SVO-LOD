#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class ServiceConfig : public ConfigService {
public:
	ServiceConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ServiceConfig";
		return className;
	}
	std::vector<std::string> services_init;
	std::vector<std::string> services_render_order;
	std::vector<std::string> services_update_order;
};
