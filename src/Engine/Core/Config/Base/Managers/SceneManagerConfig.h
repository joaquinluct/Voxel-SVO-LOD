#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class SceneManagerConfig : public ConfigBase {
public:
	SceneManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "SceneManagerConfig";
		return className;
	}
};
