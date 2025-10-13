#pragma once

#include "ConfigBase.h"
#include <string>

class PassConfigBase : public ConfigBase
{
public:
    PassConfigBase() :
        ConfigBase(),
        id(0),
        enabled(false),
        shader_name(""),
        shader_default(""),
        rasterizedState(""),
        viewPortState(""),
        stencilState(""),
        stencilDef(""),
        shaderViewState(""),
        shadowTextureView(false),
        blendState(""),
        primitiveTopology(0),
        resolution(0)
    {
        type = "PassConfigBase";
    }
    virtual ~PassConfigBase() = default;
    int id = 0;
    bool enabled = false;
    std::string shader_name = "";
    std::string shader_default = "";
    std::string rasterizedState = "";
    std::string viewPortState = "";
    std::string stencilState = "";
    std::string stencilDef = "";
    std::string shaderViewState = "";
    bool shadowTextureView = false;
    std::string blendState = "";
    int primitiveTopology = 0;
    int resolution = 0;
};
