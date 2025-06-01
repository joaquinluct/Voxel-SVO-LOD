#pragma once
#include <windows.h> //
#include <map>
#include <vector> //
#include <array> //
#include <directxmath.h> //
#include "../Chunks/Chunk.h" //
#include "../Chunks/ChunkManager.h" //
#include "../../Util/Utils.h" //

// Comentario: 'using namespace' en headers puede causar conflictos de nombres.
// Para proyectos grandes, es mejor calificar con DirectX::
using namespace DirectX; //

// Estructura para contener la malla generada (v�rtices �nicos + �ndices)
struct MarchingCubesMesh {
    std::vector<MarchingCubesVertex> vertices;
    std::vector<unsigned int> indices;
};

class MarchingCubes {
public:
    MarchingCubes(ChunkManager* chunkManager); //
    // Ahora devuelve MarchingCubesMesh para usar DrawIndexed
    MarchingCubesMesh GenerateMesh(Chunk& chunk); //

private:
    ChunkManager* m_chunkManager; //
    float ISO_LEVEL = 0.5f; // Nivel iso de la superficie

    // Tablas de b�squeda para Marching Cubes (declaraciones est�ticas)
    static const int edgeTable[256];
    static const int triTable[256][16];

    // Métodos auxiliares
    XMFLOAT3 InterpolateVerts(XMFLOAT3 p1, XMFLOAT3 p2, float valp1, float valp2);
    XMFLOAT3 CalculateNormal(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3);
};
