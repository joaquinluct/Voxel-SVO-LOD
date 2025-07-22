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
};