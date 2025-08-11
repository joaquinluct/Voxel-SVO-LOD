#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class UIManagerConfig : public ConfigBase {
public:
	UIManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "UIManagerConfig";
		return className;
	}
	std::string initial_screen;
	std::string textMesh;
	std::string font_path;
	int font_size;
};
