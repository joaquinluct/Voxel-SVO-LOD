#pragma once

#include <string>
#include <vector>
#include <map>
#include <PassConfigBase.h>

class ShadowPassConfig : public PassConfigBase {
public:
	ShadowPassConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ShadowPassConfig";
		return className;
	}
	int id;
	bool enabled;
};
