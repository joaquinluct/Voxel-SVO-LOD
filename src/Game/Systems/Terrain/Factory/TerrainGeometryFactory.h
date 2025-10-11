#pragma once

#include <Game/Systems/Terrain/Defines/TerrainGeometryEngineBase.h>
#include <string>
#include <unordered_map>
#include <vector>

class TerrainGeometryFactory {
public:
    TerrainGeometryFactory() = default;
    ~TerrainGeometryFactory() = default;
    void RegisterEngine(const std::string& name, TerrainGeometryEngineBase* engine) {
        m_engines[name] = engine;
    }
private:
    std::unordered_map<std::string, TerrainGeometryEngineBase*> m_engines;
};
