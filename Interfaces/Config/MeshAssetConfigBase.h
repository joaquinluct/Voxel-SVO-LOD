#pragma once

#include <string>
#include <vector>
#include <ConfigBase.h>

class MeshAssetConfigBase : public ConfigBase
{
public:	
	std::string shader;
	std::string mesh_path;
	std::string texture;
	std::string vertexDef;
	std::string name;
	bool cast_shadows = false;
	bool receive_shadows = false;
	std::vector<float> position = {0.0f, 0.0f, 0.0f};
};
