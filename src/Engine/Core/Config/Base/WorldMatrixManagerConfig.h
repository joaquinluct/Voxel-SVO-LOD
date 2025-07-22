#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class WorldMatrixManagerConfig : public ConfigBase {
public:
	WorldMatrixManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "WorldMatrixManagerConfig";
		return className;
	}
	int max_instances_per_batch;
};
