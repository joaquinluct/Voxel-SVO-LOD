#pragma once

#include "ConfigBase.h"
#include <string>

class PassConfigBase : public ConfigBase
{
public:
    virtual ~PassConfigBase() = default;
    int id;
    bool enabled;
    std::string shader_name;
    std::string shader_default;
    std::string rasterizedState;
    std::string viewPortState;
    std::string stencilState;
    std::string stencilDef;
    std::string shaderViewState;
    bool shadowTextureView;
    std::string blendState;
    int primitiveTopology;
    int resolution;
};
