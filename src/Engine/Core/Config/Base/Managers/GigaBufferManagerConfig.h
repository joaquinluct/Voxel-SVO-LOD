#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigBase.h>

class GigaBufferManagerConfig : public ConfigBase {
public:
	GigaBufferManagerConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "GigaBufferManagerConfig";
		return className;
	}
};
