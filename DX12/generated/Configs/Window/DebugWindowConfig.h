#pragma once

#include <string>
#include <vector>
#include <map>
#include <IMainWindowConfig.h>

class DebugWindowConfig : public IMainWindowConfig {
public:
	DebugWindowConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "DebugWindowConfig";
		return className;
	}
};
