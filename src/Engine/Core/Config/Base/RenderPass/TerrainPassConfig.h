#pragma once

#include <string>
#include <vector>
#include <map>
#include <PassConfigBase.h>

class TerrainPassConfig : public PassConfigBase {
public:
	TerrainPassConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "TerrainPassConfig";
		return className;
	}
};
