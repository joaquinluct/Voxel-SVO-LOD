#pragma once
#include <string>
#include <memory>
#include <ConfigBase.h>
#include "ITextureMapConfig.h"

struct ITextureConfig: public ConfigBase
{
	virtual ~ITextureConfig() = default;
	std::string map;
	std::shared_ptr<ITextureMapConfig> mapConfig;
	std::string texture_desc;
	float x_scale = 1.0f;
	float y_scale = 1.0f;
	float x_offset = 0.0f;
	float y_offset = 0.0f;
};