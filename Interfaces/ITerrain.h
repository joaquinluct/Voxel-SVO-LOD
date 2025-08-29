#pragma once
#include <Windows.h>
#include <Defines/WorldTerrain.h>

class ITerrain
{
public:
	virtual ~ITerrain() = default;
	// Initializes the terrain with the given width and height
	virtual HRESULT Init(WorldTerrain::TerrainDefinition terrainDefinition) = 0;
	// Updates the terrain based on the elapsed time
	virtual void Update(float deltaTime) = 0;
	// Renders the terrain to the screen
	virtual void Render() const = 0;
	// Returns the height at a specific point (x, z)
	virtual float GetHeightAt(float x, float z) const = 0;
};
