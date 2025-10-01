#pragma once

#include <string>
#include <vector>
#include <map>
#include <PassConfigBase.h>

class ReflectionConfig : public PassConfigBase {
public:
	ReflectionConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ReflectionConfig";
		return className;
	}
};
