#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class FrameStateServiceConfig : public ConfigService {
public:
	FrameStateServiceConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "FrameStateServiceConfig";
		return className;
	}
};
