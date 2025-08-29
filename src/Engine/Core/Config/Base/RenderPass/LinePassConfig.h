#pragma once

#include <string>
#include <vector>
#include <map>
#include <PassConfigBase.h>

class LinePassConfig : public PassConfigBase {
public:
	LinePassConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "LinePassConfig";
		return className;
	}
	int id;
	bool enabled;
	std::string shader_default;
};
