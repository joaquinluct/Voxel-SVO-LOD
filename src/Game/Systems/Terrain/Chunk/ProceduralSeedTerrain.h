#pragma once

#include <ITerrain.h>
#include <Defines/WorldTerrain.h>

class ProceduralSeedTerrain : public ITerrain {
private:
	WorldTerrain::TerrainDefinition m_def;
public:
	HRESULT Init(WorldTerrain::TerrainDefinition terrainDefinition);
};