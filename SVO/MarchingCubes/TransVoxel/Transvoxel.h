#pragma once
#define NOMINMAX 
#include <windows.h>
#include <map>
#include <vector>
#include <array>
#include <DirectXMath.h>
#include "..\TransVoxel\Transvoxel.h"
#include "..\MarchingCubes.h"
#include "..\MarchingCubesTables.h"
#include "TransvoxelTables.h"
#include "..\MarchingCubesMesh.h"

class World;
class VoxelData;

using namespace DirectX;

class Transvoxel {
public:
	Transvoxel() = default;
    void GenerateTransitionMesh(int face, const SVO_Node* node, const XMFLOAT3& origin, float size, const VoxelData* voxelData, MarchingCubesMesh& mesh, World* world, int ISO_LEVEL);

    void GetTransitionCellDensities(
        int face,
        const SVO_Node* node,
        const XMFLOAT3& origin,
        float size,
        const VoxelData* voxelData,
        float densities[8],
        World* world);

    void GetTransitionEdgeVerts(
        int face,
        int edge,
        const SVO_Node* node,
        const XMFLOAT3& origin,
        float size,
        const VoxelData* voxelData,
        XMFLOAT3& p1, XMFLOAT3& p2,
        float& d1, float& d2,
        World* world);

    XMFLOAT3 ComputeTransitionCornerPosition(int face, int corner, const XMFLOAT3& origin, float size);
    int GetTransitionEdgeCornerA(int face, int edge);
    int GetTransitionEdgeCornerB(int face, int edge);
};
