#pragma once
#include <Windows.h>

//#include <Defines/WorldTerrain.h>

namespace WorldTerrain {
    // Declaración anticipada de la struct anidada
    struct TerrainProceduralDefinition;
}

class IProceduralEngine
{
public:
    virtual ~IProceduralEngine() = default;
    virtual HRESULT Init(const int& seed, const WorldTerrain::TerrainProceduralDefinition& def) = 0;
    virtual const float GetHeight(const float& x, const float& z) const = 0;
};
