#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class ChunkServiceConfig : public ConfigService {
public:
	ChunkServiceConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "ChunkServiceConfig";
		return className;
	}
};
