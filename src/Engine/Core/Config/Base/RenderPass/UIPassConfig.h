#pragma once

#include <string>
#include <vector>
#include <map>
#include <PassConfigBase.h>

class UIPassConfig : public PassConfigBase {
public:
	UIPassConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "UIPassConfig";
		return className;
	}
	int id;
	bool enabled;
	std::string shader_default;
};
