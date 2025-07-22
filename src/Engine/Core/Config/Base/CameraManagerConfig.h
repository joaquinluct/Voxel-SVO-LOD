#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class CameraManagerConfig : public ConfigBase {
public:
	CameraManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "CameraManagerConfig";
		return className;
	}
	std::string initialCamera;
	std::vector<std::string> cameras;
	std::vector<std::string> camerasTypes;
};
