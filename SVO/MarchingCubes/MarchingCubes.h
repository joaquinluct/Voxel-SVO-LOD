#pragma once
#include <windows.h>
#include <map>
#include <vector>
#include <array>
#include <DirectXMath.h>
#include <unordered_map>
#include "../SVOBase/SVO_Node.h"
#include "../../Util/Utils.h"
#include "MarchingCubesTables.h"
#include "TransVoxel/Transvoxel.h"
#include "TransVoxel/TransvoxelTables.h"
#include "MarchingCubesMesh.h"
#include "../World/World.h"
#include "../../Util/DirectXUtils.h"

class VoxelData;
class Transvoxel;

// Comentario: 'using namespace' en headers puede causar conflictos de nombres.
// Para proyectos grandes, es mejor calificar con DirectX::
using namespace DirectX;

class MarchingCubes {
public:
    MarchingCubes();
	~MarchingCubes();
    // Genera la malla para un nodo SVO hoja, dado su origen, tamaño y flags de transición LOD
    MarchingCubesMesh GenerateMesh(const SVO_Node* node, const XMFLOAT3& origin, float size, const VoxelData* voxelData, const std::array<bool, 6>& lodTransitions, World* world);

private:
	Transvoxel* transvoxel;
    float ISO_LEVEL = 0.5f; // Nivel iso de la superficie
    const void GenerateTriangles(int cubeIndex, const float size, const XMFLOAT3& origin, float density[8], MarchingCubesMesh& mesh, const SVO_Node* node, const VoxelData* voxelData, World* world);

};
