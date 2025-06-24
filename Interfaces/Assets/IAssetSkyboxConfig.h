#pragma once
#include <string>
#include "IAssetSkyboxConfigBase.h"

struct IAssetSkyboxConfig : public IAssetSkyboxConfigBase
{	
	~IAssetSkyboxConfig() override = default;
	std::string type;
	std::string cubemap_path;
	std::string mesh_name;
	std::string shader_name;
	float rotation_speed{.05f};

	std::string GetType() const override { return type; }
};
