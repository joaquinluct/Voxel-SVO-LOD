#pragma once

#include <Defines/WorldTerrain.h>
#include <IProceduralEngine.h>
#include <Windows.h>

class ProceduralEngineBase : public IProceduralEngine {
private:
    int m_seed;
public:
    ProceduralEngineBase() : m_seed(0) {}
    ~ProceduralEngineBase() {};
    virtual HRESULT Init(const int& seed, const WorldTerrain::TerrainProceduralDefinition& def) override { return S_OK; }
    virtual const float GetHeight(const float& x, const float& z) const override = 0;
};
