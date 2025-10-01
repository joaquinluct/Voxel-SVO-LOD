#pragma once

#include <string>
#include <vector>
#include <map>
#include <PassConfigBase.h>

class RefractionConfig : public PassConfigBase {
public:
	RefractionConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "RefractionConfig";
		return className;
	}
};
