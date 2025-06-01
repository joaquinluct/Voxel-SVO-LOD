#pragma once
#include <d3d11.h>
#include "../VoxelMesh/VoxelMesh.h"

class OctreeRenderer
{
public:
	OctreeRenderer(DeviceManager* device, ID3D11DeviceContext* context);
	~OctreeRenderer();
	void render();

private:
	MarchingCubes* marchingCubes;
	VoxelMesh* voxelMesh;

};
