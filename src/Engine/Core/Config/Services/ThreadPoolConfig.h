#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class ThreadPoolConfig : public ConfigService {
public:
	ThreadPoolConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ThreadPoolConfig";
		return className;
	}
};
