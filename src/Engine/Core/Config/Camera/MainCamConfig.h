#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class MainCamConfig : public ConfigBase {
public:
	MainCamConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "MainCamConfig";
		return className;
	}
	bool active;
	float move_speed;
	float rotation_speed;
	float fov_y;
	float near_clip;
	float far_clip;
	std::vector<float> position;
	float initial_pitch;
	float initial_yaw;
};
