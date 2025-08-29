#include "ProceduralSeedTerrain.h"

HRESULT ProceduralSeedTerrain::Init(WorldTerrain::TerrainDefinition terrainDefinition)
{
	m_def = terrainDefinition;	

	return S_OK;
}
