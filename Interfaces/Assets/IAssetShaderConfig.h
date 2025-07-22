#pragma once
#include <string>
#include <ConfigBase.h>

class IAssetShaderConfig: public ConfigBase
{
public:
	std::string shader_path;
	std::string vertex_def;
	std::vector<std::string> matrix_slots;
	std::string name;
};
