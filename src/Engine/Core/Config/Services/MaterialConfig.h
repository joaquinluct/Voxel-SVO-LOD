#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class MaterialConfig : public ConfigService {
public:
	MaterialConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "MaterialConfig";
		return className;
	}
};
