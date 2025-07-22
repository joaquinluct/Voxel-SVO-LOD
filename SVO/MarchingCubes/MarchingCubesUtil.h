#pragma once
#include <DirectXMath.h>
#include "../SVOBase/SVO_Node.h"
#include "../Voxel/VoxelData.h"

class World;

using namespace DirectX;

namespace MarchingCubesUtil {
    // Consulta la densidad en una posición del mundo (SVO traversal)
    float GetDensityAtWorldPos(const XMFLOAT3& pos, const SVO_Node* node, const VoxelData* voxelData, const XMFLOAT3& origin, float nodeSize, World* world);    // Interpola dos vértices dados sus posiciones y densidades
    XMFLOAT3 InterpolateVerts(XMFLOAT3 p1, XMFLOAT3 p2, float valp1, float valp2, int ISO_LEVEL);
    XMFLOAT3 InterpolateVerts2(XMFLOAT3 p1, XMFLOAT3 p2, float valp1, float valp2, int ISO_LEVEL);
    // Calcula la normal de densidad en una posición del mundo
    XMFLOAT3 CalculateNormal(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3);
    XMFLOAT3 CalculateDensityNormal(const XMFLOAT3& pos, const SVO_Node* node, const VoxelData* voxelData, const XMFLOAT3& origin, float nodeSize, World* world);
    int GetCubeIndex(float(&cube)[8], XMFLOAT3 origin, float size, const SVO_Node* node, const VoxelData* voxelData, World* world, float ISO_LEVEL);
    
    // Interpolación de vértices en todas las aristas de un cubo basado en la tabla de aristas
    void InterpolateEdgeVerts(int cubeIndex, const XMFLOAT3 p_local[8], const float density[8], XMFLOAT3 (&vertlist)[12], int ISO_LEVEL);

    float TrilinearInterpolate(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& nodeOrigin, float nodeSize, const float cornerDensities[8]);
};
