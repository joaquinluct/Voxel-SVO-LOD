#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class ProceduralServiceConfig : public ConfigService {
public:
	ProceduralServiceConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ProceduralServiceConfig";
		return className;
	}
};
