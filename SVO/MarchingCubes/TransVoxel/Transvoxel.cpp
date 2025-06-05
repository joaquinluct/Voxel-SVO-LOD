#include "Transvoxel.h"
#include "../MarchingCubesTables.h"
#include "TransvoxelTables.h"
#include "../../World/World.h"
#include "../MarchingCubesUtil.h"
#include <DirectXMath.h>
#include <iostream>
#include <cmath>

using namespace DirectX;

// Lógica real de Transvoxel
// -------------------------
// 1. Determinar la cuadrícula de transición para la cara 'face'
// 2. Leer los 8 valores de densidad de la "corteza" de transición (del nodo y vecino)
// 3. Calcular el caseIndex para la tabla de transición
// 4. Consultar la tabla Transvoxel y generar los triángulos
// ------------------------------------------------------------------------------------

void Transvoxel::GenerateTransitionMesh(int face, const SVO_Node* node, const XMFLOAT3& origin, float size, const VoxelData* voxelData, MarchingCubesMesh& mesh, World* world, int ISO_LEVEL) {
    
    float densities[8];

    GetTransitionCellDensities(face, node, origin, size, voxelData, densities, world);
    int caseIndex = 0;
    for (int i = 0; i < 8; ++i) {
        if (densities[i] >= ISO_LEVEL) caseIndex |= (1 << i);
    }
    int classIndex = TransvoxelTables::transitionCellClass[caseIndex] & 0x7F;
    bool invert = (TransvoxelTables::transitionCellClass[caseIndex] & 0x80) != 0;
    if (classIndex >= 0 && classIndex < sizeof(TransvoxelTables::transitionCellData) / sizeof(TransvoxelTables::transitionCellData[0])) {
        const TransvoxelTables::TransitionCellData& cell = TransvoxelTables::transitionCellData[classIndex];
        XMFLOAT3 vertlist[13]{};
        for (int e = 0; e < 13; ++e) {
            XMFLOAT3 p1, p2;
            float d1, d2;
            GetTransitionEdgeVerts(face, e, node, origin, size, voxelData, p1, p2, d1, d2, world);
            vertlist[e] = MarchingCubesUtil::InterpolateVerts(p1, p2, d1, d2, ISO_LEVEL);
        }
        for (int t = 0; t < cell.GetTriangleCount(); ++t) {
            int i0 = cell.vertexIndex[t * 3 + 0];
            int i1 = cell.vertexIndex[t * 3 + 1];
            int i2 = cell.vertexIndex[t * 3 + 2];
            if (invert) std::swap(i1, i2);
            MarchingCubesVertex v[3];
            v[0].Position = vertlist[i0];
            v[1].Position = vertlist[i1];
            v[2].Position = vertlist[i2];
            XMFLOAT3 normal = MarchingCubesUtil::CalculateNormal(v[0].Position, v[1].Position, v[2].Position);
            v[0].Normal = v[1].Normal = v[2].Normal = normal;
            v[0].Color = v[1].Color = v[2].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            int baseIndex = (int)mesh.vertices.size();
            mesh.vertices.push_back(v[0]);
            mesh.vertices.push_back(v[1]);
            mesh.vertices.push_back(v[2]);
            mesh.indices.push_back(baseIndex);
            mesh.indices.push_back(baseIndex + 1);
            mesh.indices.push_back(baseIndex + 2);
        }
    } else {
        // std::cerr << "Error: classIndex fuera de rango en transitionCellData." << std::endl;
    }
}

void Transvoxel::GetTransitionCellDensities(
    int face,
    const SVO_Node* node,
    const XMFLOAT3& origin,
    float size,
    const VoxelData* voxelData,
    float densities[8],
    World* world)
{
    for (int i = 0; i < 8; ++i) {
        XMFLOAT3 cornerPos = ComputeTransitionCornerPosition(face, i, origin, size);
        densities[i] = MarchingCubesUtil::GetDensityAtWorldPos(cornerPos, node, voxelData, origin, size, world);
    }
}

void Transvoxel::GetTransitionEdgeVerts(
    int face,
    int edge,
    const SVO_Node* node,
    const XMFLOAT3& origin,
    float size,
    const VoxelData* voxelData,
    XMFLOAT3& p1, XMFLOAT3& p2,
    float& d1, float& d2,
    World* world)
{
    int cornerA = GetTransitionEdgeCornerA(face, edge);
    int cornerB = GetTransitionEdgeCornerB(face, edge);
    p1 = ComputeTransitionCornerPosition(face, cornerA, origin, size);
    p2 = ComputeTransitionCornerPosition(face, cornerB, origin, size);
    d1 = MarchingCubesUtil::GetDensityAtWorldPos(p1, node, voxelData, origin, size, world);
    d2 = MarchingCubesUtil::GetDensityAtWorldPos(p2, node, voxelData, origin, size, world);
}

XMFLOAT3 Transvoxel::ComputeTransitionCornerPosition(int face, int corner, const XMFLOAT3& origin, float size)
{    
    int dx = TransvoxelTables::transitionCornerOffsets[face][corner][0];
    int dy = TransvoxelTables::transitionCornerOffsets[face][corner][1];
    int dz = TransvoxelTables::transitionCornerOffsets[face][corner][2];
    float step = size / 2.0f;
    XMFLOAT3 pos = origin;
    pos.x += dx * step;
    pos.y += dy * step;
    pos.z += dz * step;
    return pos;
}

int Transvoxel::GetTransitionEdgeCornerA(int face, int edge)
{
    return TransvoxelTables::transitionEdgeCorners[edge][0];
}
int Transvoxel::GetTransitionEdgeCornerB(int face, int edge)
{
    return TransvoxelTables::transitionEdgeCorners[edge][1];
}
