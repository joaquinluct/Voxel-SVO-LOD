#pragma once

#include <ConfigBase.h>

struct ITextureMapConfig : public ConfigBase
{
	virtual ~ITextureMapConfig() = default;
	std::string texture_type;
	std::string albedo;
	std::string normal;
	std::string roughness;
	std::string metallic;
	std::string ao;
};

