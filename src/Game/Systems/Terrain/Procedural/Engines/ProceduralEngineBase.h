#pragma once

#include <Windows.h>
#include <IProceduralEngine.h>
#include "FastNoiseLite/FastNoiseLite.h"
#include <Defines/WorldTerrain.h>

//namespace WorldTerrain {
//	// Declaración anticipada de la struct anidada
//	struct TerrainProceduralDefinition;
//}

class ProceduralEngineBase : public IProceduralEngine {
private:
	int m_seed;
public:
	ProceduralEngineBase() : m_seed(0) {}
	~ProceduralEngineBase() override {}
	HRESULT Init() override { return S_OK; };
	HRESULT Init(const int& seed, const WorldTerrain::TerrainProceduralDefinition& def) override { return S_OK; };
	const float GetHeight(const float& x, const float& z) const override;
	//const float GetHeight(const float& x, const float& z) const override;
};