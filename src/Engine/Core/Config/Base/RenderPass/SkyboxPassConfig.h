#pragma once

#include <string>
#include <vector>
#include <map>
#include <PassConfigBase.h>

class SkyboxPassConfig : public PassConfigBase {
public:
	SkyboxPassConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "SkyboxPassConfig";
		return className;
	}
};
