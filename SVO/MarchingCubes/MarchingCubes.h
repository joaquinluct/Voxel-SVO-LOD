#pragma once
#include <windows.h>
#include <map>
#include <vector>
#include <array>
#include <DirectXMath.h>
#include <unordered_map>
#include "../SVOBase/SVO_Node.h"
#include "../Voxel/VoxelData.h"
#include "../../Util/Utils.h"
#include "MarchingCubesTables.h"
#include "TransvoxelTables.h"

// Comentario: 'using namespace' en headers puede causar conflictos de nombres.
// Para proyectos grandes, es mejor calificar con DirectX::
using namespace DirectX; //

// Estructura para contener la malla generada (v�rtices �nicos + �ndices)
struct MarchingCubesMesh {
    std::vector<MarchingCubesVertex> vertices;
    std::vector<unsigned int> indices;
    void addMesh(const MarchingCubesMesh& other) {
        size_t vertexOffset = vertices.size();
        vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());
        for (unsigned int originalOtherIndex : other.indices) {
            indices.push_back(originalOtherIndex + static_cast<unsigned int>(vertexOffset));
        }
	}
    void removeDuplicateVertices() {
        std::vector<MarchingCubesVertex> newVertices;
        std::vector<unsigned int> newIndices;
        std::unordered_map<MarchingCubesVertex, unsigned int> uniqueVertexMap;
        unsigned int nextIndex = 0;

        // Recorrer cada índice de la malla actual
        for (unsigned int originalIndex : indices) {
            // Obtener el vértice correspondiente de la lista actual de vértices
            const MarchingCubesVertex& currentVertex = vertices[originalIndex];

            // Intentar insertar el vértice en el mapa de vértices únicos
            // 'it' será un iterador al elemento en el mapa.
            // 'inserted' será true si el vértice se insertó (es nuevo), false si ya existía.
            auto [it, inserted] = uniqueVertexMap.insert({currentVertex, nextIndex});

            if (inserted) {
                // Si el vértice es nuevo, lo añadimos a nuestra lista de vértices únicos
                newVertices.push_back(currentVertex);
                // Incrementamos el contador para el próximo índice nuevo disponible
                nextIndex++;
            }
            // Añadimos el índice mapeado (el índice del vértice en newVertices)
            // al nuevo array de índices. Si el vértice ya existía, 'it->second'
            // contendrá el índice original de ese vértice en newVertices.
            newIndices.push_back(it->second);
        }

        // Reemplazar los arrays de vértices e índices de la malla con los nuevos
        vertices = std::move(newVertices); // Usa std::move para eficiencia
        indices = std::move(newIndices);   // Usa std::move para eficiencia
    }
};

class MarchingCubes {
public:
    MarchingCubes();
    // Genera la malla para un nodo SVO hoja, dado su origen, tamaño y flags de transición LOD
    MarchingCubesMesh GenerateMesh(const SVO_Node* node, const XMFLOAT3& origin, float size, const VoxelData* voxelData, const std::array<bool, 6>& lodTransitions, class World* world);

private:
    float ISO_LEVEL = 0.5f; // Nivel iso de la superficie
    // --- Transvoxel ---
    // Tabla de triángulos de transición (mockup, reemplazar por la real)
    static const int transitionTriTable[256][16];
    // Genera la malla de transición para un borde LOD concreto
    void GenerateTransitionMesh(int face, const SVO_Node* node, const XMFLOAT3& origin, float size, const VoxelData* voxelData, MarchingCubesMesh& mesh, class World* world);

    // --- Funciones auxiliares para Transvoxel ---
    void GetTransitionCellDensities(
        int face,
        const SVO_Node* node,
        const XMFLOAT3& origin,
        float size,
        const VoxelData* voxelData,
        float densities[8],
        class World* world);

    void GetTransitionEdgeVerts(
        int face,
        int edge,
        const SVO_Node* node,
        const XMFLOAT3& origin,
        float size,
        const VoxelData* voxelData,
        XMFLOAT3& p1, XMFLOAT3& p2,
        float& d1, float& d2,
        class World* world);

    XMFLOAT3 ComputeTransitionCornerPosition(int face, int corner, const XMFLOAT3& origin, float size);
    // Cambiado: ahora requiere origin y size para SVO traversal
    float GetDensityAtWorldPos(const XMFLOAT3& pos, const SVO_Node* node, const VoxelData* voxelData, const XMFLOAT3& origin, float size, class World* world);
    int GetTransitionEdgeCornerA(int face, int edge);
    int GetTransitionEdgeCornerB(int face, int edge);

    // --- NUEVO: normales suaves por gradiente de densidad ---
    XMFLOAT3 CalculateDensityNormal(const XMFLOAT3& pos, const SVO_Node* node, const VoxelData* voxelData, const XMFLOAT3& origin, float nodeSize, World* world);
    void GenerateTriangles(int cubeIndex, float posX, float posY, float posZ, float size, float density[8], MarchingCubesMesh& mesh, const SVO_Node* node, const VoxelData* voxelData, const XMFLOAT3& origin, float nodeSize, World* world);

    // Métodos auxiliares
    XMFLOAT3 InterpolateVerts(XMFLOAT3 p1, XMFLOAT3 p2, float valp1, float valp2);
    XMFLOAT3 InterpolateVerts2(XMFLOAT3 p1, XMFLOAT3 p2, float valp1, float valp2);
    XMFLOAT3 CalculateNormal(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3);
    void GenerateTriangles(int cubeIndex, float x, float, float z, float size, float density[8], MarchingCubesMesh& mesh);
};
