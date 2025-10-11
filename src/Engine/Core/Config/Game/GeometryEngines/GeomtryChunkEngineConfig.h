#pragma once

#include <string>
#include <vector>
#include <map>
#include <ConfigService.h>

class GeometryChunkEngineConfig : public ConfigService {
public:
    GeometryChunkEngineConfig();
    static const std::string& GetStaticConfigName()
    {
        static const std::string className = "GeometryChunkEngineConfig";
        return className;
    }
    std::string name;
    std::string version;
    std::string terrain_asset;
    std::string description;
};
