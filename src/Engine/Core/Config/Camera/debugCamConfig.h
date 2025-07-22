#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class DebugCamConfig : public ConfigBase {
public:
	DebugCamConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "DebugCamConfig";
		return className;
	}
	bool active;
	float move_speed;
	float rotation_speed;
	float fov_y;
	float near_clip;
	float far_clip;
	std::vector<float> position;
};
