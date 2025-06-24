#include "MarchingCubes.h"
#include "MarchingCubesTables.h"
#include "MarchingCubesUtil.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <tuple>
#include "../World/World.h"

// Comparador y hash para XMFLOAT3 con tolerancia
struct XMFLOAT3Key {
    int x, y, z;
    static constexpr float TOL = 1e-4f;
    XMFLOAT3Key(const XMFLOAT3& p) {
        x = static_cast<int>(std::round(p.x / TOL));
        y = static_cast<int>(std::round(p.y / TOL));
        z = static_cast<int>(std::round(p.z / TOL));
    }
    bool operator==(const XMFLOAT3Key& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};
namespace std {
    template<>
    struct hash<XMFLOAT3Key> {
        size_t operator()(const XMFLOAT3Key& k) const {
            return ((std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1)) >> 1) ^ (std::hash<int>()(k.z) << 1);
        }
    };
}

MarchingCubes::MarchingCubes() {  
    transvoxel = new Transvoxel(); // Allocate Transvoxel dynamically  
}  

MarchingCubes::~MarchingCubes() {  
    delete transvoxel; // Ensure proper cleanup  
}

const float MarchingCubes::ISO_LEVEL = 0.0f;

const void MarchingCubes::GenerateTriangles(int cubeIndex, const float size, const XMFLOAT3& origin, float density[8], MarchingCubesMesh& mesh, const SVO_Node* node, const VoxelData* voxelData, World* world) {
    using namespace MarchingCubesTables;
    unsigned int nextIndex = 0;
    std::unordered_map<XMFLOAT3Key, unsigned int> uniqueVerticesMap;
    XMFLOAT3 p_local[8]{};
        
    for (int i = 0; i < 8; ++i) {
        p_local[i] = DirectXUtils::Add(DirectXUtils::Multiply(cornerOffsetsX[i], size), origin);
	}
    
    // Encontrar los puntos de intersección de la superficie con las aristas del cubo
    // Y almacenar las posiciones interpoladas en vertlist
    XMFLOAT3 vertlist[12]{ origin };
    // Usar la función utilitaria para interpolar vértices en las aristas
    MarchingCubesUtil::InterpolateEdgeVerts(cubeIndex, p_local, density, vertlist, static_cast<int>(this->ISO_LEVEL));

    // Crear los triángulos usando triTable
    for (int i = 0; triTable[cubeIndex][i] != -1; i += 3) {
        MarchingCubesVertex marchingVertex[3]{};
        // Por convención DirectX: antihorario es el frente
        for (int v = 0; v < 3; ++v) {
            int idx = triTable[cubeIndex][i + v];
            marchingVertex[v].Position = vertlist[idx];
            /*marchingVertex[v].Normal = MarchingCubesUtil::CalculateDensityNormal(vertlist[idx], node, voxelData, origin, size, world);*/
            marchingVertex[v].Normal = node->GetNormal();
            marchingVertex[v].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
        }

        // --- FIX: Forzar winding antihorario para todos los triángulos ---
        // std::swap(marchingVertex[1], marchingVertex[2]);
        // --- FIN FIX ---
        // Añadir vértices e índices, manejando duplicados
        for (int j = 0; j < 3; ++j) {
            XMFLOAT3Key key(marchingVertex[j].Position);
            auto it = uniqueVerticesMap.find(key);
            if (it == uniqueVerticesMap.end()) {
                uniqueVerticesMap[key] = nextIndex;
                mesh.vertices.push_back(marchingVertex[j]);
                mesh.indices.push_back(nextIndex);
                nextIndex++;
            } else {
                mesh.indices.push_back(it->second);
            }
        }
    }
}

MarchingCubesMesh MarchingCubes::GenerateMesh(const SVO_Node* node, const XMFLOAT3& origin, const float size, const VoxelData* voxelData, const std::array<bool, 6>& lodTransitions, World* world) {
        
    MarchingCubesMesh mesh;
	//if (!node || !node->IsLeaf() || !node->IsOccupied()) return mesh;
    if (!node || !node->IsOccupied()) return mesh;
    // if (!node) return mesh;
    // Para cada vértice del cubo, consulta la densidad usando el SVO
    // float cube[8]{};
    const float* cubePtr = node->GetCube();
    float cube[8];  
    std::copy(cubePtr, cubePtr + 8, cube);
	int cubeIndex = MarchingCubesUtil::GetCubeIndex(cube, origin, size, node, voxelData, world, ISO_LEVEL);
    /*float cube[8]{0.001f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};
    int cubeIndex = 1;*/
    /*if (cubeIndex < 129) {
        return mesh;
    }*/

    int edges = MarchingCubesTables::edgeTable[cubeIndex];
    if (edges == 0) return mesh;
    // Interpola vértices y genera triángulos (lógica estándar de Marching Cubes)
    // ... (tu lógica de interpolación y generación de triángulos aquí) ...
    GenerateTriangles(cubeIndex, size, origin, cube, mesh, node, voxelData, world);

    // --- INICIO: Lógica de transición LOD ---
    /*for (int face = 0; face < 6; ++face) {
        if (lodTransitions[face]) {
            transvoxel->GenerateTransitionMesh(face, node, origin, size, voxelData, mesh, world, ISO_LEVEL);
        }
    }*/
    // --- FIN: Lógica de transición LOD ---
    return mesh;
}
