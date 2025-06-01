#include "MarchingCubes.h"
#include "../World/World.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <tuple>

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

// Genera la malla de transición para un borde LOD concreto (mockup)
void MarchingCubes::GenerateTransitionMesh(int face, const SVO_Node* node, const XMFLOAT3& origin, float size, const VoxelData* voxelData, MarchingCubesMesh& mesh, World* world) {
    // --- INICIO: Lógica real de Transvoxel ---
    // 1. Determinar la cuadrícula de transición para la cara 'face'
    // 2. Leer los 8 valores de densidad de la "corteza" de transición (del nodo y vecino)
    // 3. Calcular el caseIndex para la tabla de transición
    // 4. Consultar la tabla Transvoxel y generar los triángulos

    // Ejemplo de estructura básica (debes adaptar la consulta de densidades y la interpolación a tu sistema):
    using namespace Transvoxel;
    
    // 1. Obtener los 8 valores de densidad para la celda de transición (debes implementar GetTransitionCellDensities)
    float densities[8];
    GetTransitionCellDensities(face, node, origin, size, voxelData, densities, world); // <-- Debes implementar esta función

    // Cambia las llamadas en GenerateMesh y helpers para pasar el puntero a World
    // 2. Calcular el caseIndex (igual que Marching Cubes, pero para la celda de transición)
    int caseIndex = 0;
    for (int i = 0; i < 8; ++i) {
        if (densities[i] > ISO_LEVEL) caseIndex |= (1 << i);
    }

    // 3. Consultar la tabla de clases de celda de transición
    int classIndex = transitionCellClass[caseIndex] & 0x7F;
    bool invert = (transitionCellClass[caseIndex] & 0x80) != 0;

    
	// Verificar límites antes de acceder a transitionCellData
	if (classIndex >= 0 && classIndex < sizeof(transitionCellData) / sizeof(transitionCellData[0])) {
		const TransitionCellData& cell = transitionCellData[classIndex];		
		// 4. Generar los vértices de la celda de transición
		XMFLOAT3 vertlist[13]{}; // 13 posibles vértices por celda de transición
		for (int e = 0; e < 13; ++e) {
			// Interpola los vértices de borde según la topología de Transvoxel
			// Debes implementar GetTransitionEdgeVerts(face, e, ...)

			XMFLOAT3 p1, p2;
			float d1, d2;
			GetTransitionEdgeVerts(face, e, node, origin, size, voxelData, p1, p2, d1, d2, world); // <-- Debes implementar
			vertlist[e] = InterpolateVerts(p1, p2, d1, d2);
		}

		// 5. Añadir triángulos
		for (int t = 0; t < cell.GetTriangleCount(); ++t) {
			int i0 = cell.vertexIndex[t * 3 + 0];
			int i1 = cell.vertexIndex[t * 3 + 1];
			int i2 = cell.vertexIndex[t * 3 + 2];
			if (invert) std::swap(i1, i2);
			MarchingCubesVertex v[3];
			v[0].Position = vertlist[i0];
			v[1].Position = vertlist[i1];
			v[2].Position = vertlist[i2];
			// Calcular normales
			XMFLOAT3 normal = CalculateNormal(v[0].Position, v[1].Position, v[2].Position);
			v[0].Normal = v[1].Normal = v[2].Normal = normal;
			v[0].Color = v[1].Color = v[2].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // Color blanco por defecto
			// Añadir a la malla
			int baseIndex = (int)mesh.vertices.size();
			mesh.vertices.push_back(v[0]);
			mesh.vertices.push_back(v[1]);
			mesh.vertices.push_back(v[2]);
			mesh.indices.push_back(baseIndex);
			mesh.indices.push_back(baseIndex + 1);
			mesh.indices.push_back(baseIndex + 2);
		}
	}
	else {
		// Manejo de error o comportamiento predeterminado
		std::cerr << "Error: classIndex fuera de rango en transitionCellData." << std::endl;
	}
    // --- FIN: Lógica real de Transvoxel ---
}

MarchingCubes::MarchingCubes() {}

void MarchingCubes::GenerateTriangles(int cubeIndex, float posX, float posY, float posZ, float size, float density[8], MarchingCubesMesh& mesh, const SVO_Node* node, const VoxelData* voxelData, const XMFLOAT3& origin, float nodeSize, World* world) {
    using namespace MarchingCubesTables;
    unsigned int nextIndex = 0;
    std::unordered_map<XMFLOAT3Key, unsigned int> uniqueVerticesMap;
    XMFLOAT3 p_local[8]{};
    float x = posX, y = posY, z = posZ;
    p_local[0] = XMFLOAT3(x, y, z);
    p_local[1] = XMFLOAT3(x + 1, y, z);
    p_local[2] = XMFLOAT3(x + 1, y + 1, z);
    p_local[3] = XMFLOAT3(x, y + 1, z);
    p_local[4] = XMFLOAT3(x, y, z + 1);
    p_local[5] = XMFLOAT3(x + 1, y, z + 1);
    p_local[6] = XMFLOAT3(x + 1, y + 1, z + 1);
    p_local[7] = XMFLOAT3(x, y + 1, z + 1);

    // Calcula los corners en coordenadas de mundo usando origin y size
    static const int cornerOffsets[8][3] = {
        {0,0,0},{1,0,0},{1,1,0},{0,1,0},
        {0,0,1},{1,0,1},{1,1,1},{0,1,1}
    };
    for (int i = 0; i < 8; ++i) {
        p_local[i] = XMFLOAT3(
            x + cornerOffsets[i][0] * size,
            y + cornerOffsets[i][1] * size,
            z + cornerOffsets[i][2] * size
        );
    }

    // Encontrar los puntos de intersección de la superficie con las aristas del cubo
    // Y almacenar las posiciones interpoladas en vertlist
    XMFLOAT3 vertlist[12]{ XMFLOAT3(0,0,0) };
    if (edgeTable[cubeIndex] & 0x001) vertlist[0] = InterpolateVerts2(p_local[0], p_local[1], density[0], density[1]);
    if (edgeTable[cubeIndex] & 0x002) vertlist[1] = InterpolateVerts2(p_local[1], p_local[2], density[1], density[2]);
    if (edgeTable[cubeIndex] & 0x004) vertlist[2] = InterpolateVerts2(p_local[2], p_local[3], density[2], density[3]);
    if (edgeTable[cubeIndex] & 0x008) vertlist[3] = InterpolateVerts2(p_local[3], p_local[0], density[3], density[0]);
    if (edgeTable[cubeIndex] & 0x010) vertlist[4] = InterpolateVerts2(p_local[4], p_local[5], density[4], density[5]);
    if (edgeTable[cubeIndex] & 0x020) vertlist[5] = InterpolateVerts2(p_local[5], p_local[6], density[5], density[6]);
    if (edgeTable[cubeIndex] & 0x040) vertlist[6] = InterpolateVerts2(p_local[6], p_local[7], density[6], density[7]);
    if (edgeTable[cubeIndex] & 0x080) vertlist[7] = InterpolateVerts2(p_local[7], p_local[4], density[7], density[4]);
    if (edgeTable[cubeIndex] & 0x100) vertlist[8] = InterpolateVerts2(p_local[0], p_local[4], density[0], density[4]);
    if (edgeTable[cubeIndex] & 0x200) vertlist[9] = InterpolateVerts2(p_local[1], p_local[5], density[1], density[5]);
    if (edgeTable[cubeIndex] & 0x400) vertlist[10] = InterpolateVerts2(p_local[2], p_local[6], density[2], density[6]);
    if (edgeTable[cubeIndex] & 0x800) vertlist[11] = InterpolateVerts2(p_local[3], p_local[7], density[3], density[7]);

    // Elimina el bucle que multiplicaba por size (ya está aplicado)
    // for (int i = 0; i < 12; ++i) {
    //     vertlist[i].x *= size;
    //     vertlist[i].y *= size;
    //     vertlist[i].z *= size;
    // }

    // LOG: Triángulos generados
    char triLog[256];
    sprintf_s(triLog, sizeof(triLog), "[MarchingCubes::GenerateTriangles] cubeIndex=%d pos=(%d,%d,%d) numTris=%d\n", cubeIndex, posX, posY, posZ, (int)(sizeof(triTable[cubeIndex])/sizeof(int)));
    OutputDebugStringA(triLog);
    // Crear los triángulos usando triTable
    for (int i = 0; triTable[cubeIndex][i] != -1; i += 3) { //
        MarchingCubesVertex marchingVertex[3]{}; //
        // INVERSIÓN DEL ORDEN DE LOS VÉRTICES PARA CORREGIR NORMALES
        for (int v = 0; v < 3; ++v) {
            int idx = (v == 0) ? triTable[cubeIndex][i] : (v == 1) ? triTable[cubeIndex][i + 2] : triTable[cubeIndex][i + 1];
            marchingVertex[v].Position = vertlist[idx];
            // Calcular normal suave por gradiente de densidad
            marchingVertex[v].Normal = CalculateDensityNormal(vertlist[idx], node, voxelData, origin, nodeSize, world);
            marchingVertex[v].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
        }

        // LOG: Vértices del triángulo
        char vtxLog[256];
        sprintf_s(vtxLog, sizeof(vtxLog), "[MarchingCubes::GenerateTriangles] v0=(%.2f,%.2f,%.2f) v1=(%.2f,%.2f,%.2f) v2=(%.2f,%.2f,%.2f)\n",
            marchingVertex[0].Position.x, marchingVertex[0].Position.y, marchingVertex[0].Position.z,
            marchingVertex[1].Position.x, marchingVertex[1].Position.y, marchingVertex[1].Position.z,
            marchingVertex[2].Position.x, marchingVertex[2].Position.y, marchingVertex[2].Position.z);
        OutputDebugStringA(vtxLog);

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

MarchingCubesMesh MarchingCubes::GenerateMesh(const SVO_Node* node, const XMFLOAT3& origin, float size, const VoxelData* voxelData, const std::array<bool, 6>& lodTransitions, World* world) {

    using namespace MarchingCubesTables;

    MarchingCubesMesh mesh;
	//if (!node || !node->IsLeaf() || !node->IsOccupied()) return mesh;
    if (!node || !node->IsOccupied()) return mesh;
    // Para cada vértice del cubo, consulta la densidad usando el SVO
    float cube[8]{};
    static const int cornerOffsets[8][3] = {
        {0,0,0},{1,0,0},{1,1,0},{0,1,0},
        {0,0,1},{1,0,1},{1,1,1},{0,1,1}
    };
    for (int i = 0; i < 8; ++i) {
        XMFLOAT3 cornerPos = {
            origin.x + cornerOffsets[i][0] * size,
            origin.y + cornerOffsets[i][1] * size,
            origin.z + cornerOffsets[i][2] * size
        };
        cube[i] = GetDensityAtWorldPos(cornerPos, node, voxelData, origin, size, world);
        // LOG: Densidad de cada corner
        // char logMsg[256];
        // sprintf_s(logMsg, sizeof(logMsg), "[MarchingCubes::GenerateMesh] Node@(%f,%f,%f) size=%f corner[%d] pos=(%f,%f,%f) density=%f\n", origin.x, origin.y, origin.z, size, i, cornerPos.x, cornerPos.y, cornerPos.z, cube[i]);
        // OutputDebugStringA(logMsg);
    }
    // Calcula el índice de la tabla
    int cubeIndex = 0;
    for (int i = 0; i < 8; ++i) {
        if (cube[i] >= ISO_LEVEL) cubeIndex |= (1 << i);
    }
    // LOG: cubeIndex
    // char logMsg[128];
    // sprintf_s(logMsg, sizeof(logMsg), "[MarchingCubes::GenerateMesh] Node@(%f,%f,%f) size=%f cubeIndex=%d\n", origin.x, origin.y, origin.z, size, cubeIndex);
    // OutputDebugStringA(logMsg);

	// Si la densidad es >= 0.5f, significa que hay material (s�lido) en ese v�rtice
	// if (cube[0] >= ISO_LEVEL) cubeIndex |= 1;
	// if (cube[1] >= ISO_LEVEL) cubeIndex |= 2;
	// if (cube[2] >= ISO_LEVEL) cubeIndex |= 4;
	// if (cube[3] >= ISO_LEVEL) cubeIndex |= 8;
	// if (cube[4] >= ISO_LEVEL) cubeIndex |= 16;
	// if (cube[5] >= ISO_LEVEL) cubeIndex |= 32;
	// if (cube[6] >= ISO_LEVEL) cubeIndex |= 64;
	// if (cube[7] >= ISO_LEVEL) cubeIndex |= 128;

    int edges = edgeTable[cubeIndex];
    if (edges == 0) return mesh;
    // Interpola vértices y genera triángulos (lógica estándar de Marching Cubes)
    // ... (tu lógica de interpolación y generación de triángulos aquí) ...
    GenerateTriangles(cubeIndex, origin.x, origin.y, origin.z, size, cube, mesh, node, voxelData, origin, size, world);
    // --- INICIO: Lógica de transición LOD ---
    for (int face = 0; face < 6; ++face) {
        if (lodTransitions[face]) {
            GenerateTransitionMesh(face, node, origin, size, voxelData, mesh, world);
        }
    }
    // --- FIN: Lógica de transición LOD ---
    return mesh;
}

// --- NUEVO: Cálculo de gradiente para normales suaves por vértice ---
XMFLOAT3 MarchingCubes::CalculateDensityNormal(const XMFLOAT3& pos, const SVO_Node* node, const VoxelData* voxelData, const XMFLOAT3& origin, float nodeSize, World* world) {
    const float h = 0.5f; // Paso pequeño para diferencias centrales
    float dx = GetDensityAtWorldPos({pos.x + h, pos.y, pos.z}, node, voxelData, origin, nodeSize, world)
             - GetDensityAtWorldPos({pos.x - h, pos.y, pos.z}, node, voxelData, origin, nodeSize, world);
    float dy = GetDensityAtWorldPos({pos.x, pos.y + h, pos.z}, node, voxelData, origin, nodeSize, world)
             - GetDensityAtWorldPos({pos.x, pos.y - h, pos.z}, node, voxelData, origin, nodeSize, world);
    float dz = GetDensityAtWorldPos({pos.x, pos.y, pos.z + h}, node, voxelData, origin, nodeSize, world)
             - GetDensityAtWorldPos({pos.x, pos.y, pos.z - h}, node, voxelData, origin, nodeSize, world);
    XMVECTOR grad = XMVectorSet(dx, dy, dz, 0.0f);
    grad = XMVector3Normalize(grad);
    XMFLOAT3 normal;
    XMStoreFloat3(&normal, grad);
    // Invertir el gradiente para que apunte hacia fuera de la superficie (iso-superficie)
    normal.x = -normal.x; normal.y = -normal.y; normal.z = -normal.z;
    return normal;
}

XMFLOAT3 MarchingCubes::InterpolateVerts(XMFLOAT3 p1, XMFLOAT3 p2, float valp1, float valp2) {
	if (valp1 == valp2) return p1;
    float t = (ISO_LEVEL - valp1) / (valp2 - valp1);
    return XMFLOAT3(
        p1.x + t * (p2.x - p1.x),
        p1.y + t * (p2.y - p1.y),
        p1.z + t * (p2.z - p1.z)
    );
}

XMFLOAT3 MarchingCubes::InterpolateVerts2(XMFLOAT3 p1, XMFLOAT3 p2, float valp1, float valp2) { //
	if (abs(valp2 - valp1) < 0.00001f) { // Evita divisi�n por cero y maneja casos de misma densidad
		return p1; // Devuelve el primer punto si las densidades son casi iguales
	}
	float mu = (ISO_LEVEL - valp1) / (valp2 - valp1);
	// Asegúrate de que mu esté en el rango [0, 1]
	mu = std::clamp(mu, 0.0f, 1.0f);
	return XMFLOAT3(p1.x + mu * (p2.x - p1.x), p1.y + mu * (p2.y - p1.y), p1.z + mu * (p2.z - p1.z)); //
}

XMFLOAT3 MarchingCubes::CalculateNormal(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3) {
    XMVECTOR v1 = XMLoadFloat3(&p1);
    XMVECTOR v2 = XMLoadFloat3(&p2);
    XMVECTOR v3 = XMLoadFloat3(&p3);
    XMVECTOR n = XMVector3Cross(v2 - v1, v3 - v1);
    n = XMVector3Normalize(n);
    XMFLOAT3 normal;
    XMStoreFloat3(&normal, n);
    return normal;
}

// Función auxiliar: obtiene las densidades de los 8 corners de la celda de transición para la cara dada
void MarchingCubes::GetTransitionCellDensities(
    int face,
    const SVO_Node* node,
    const XMFLOAT3& origin,
    float size,
    const VoxelData* voxelData,
    float densities[8],
    World* world)
{
    // NOTA: Debes adaptar el mapeo de corners y la consulta de densidad a tu sistema.
    // Aquí se asume que tienes una función GetDensityAtWorldPos(XMFLOAT3 pos, ...)
    // y que sabes cómo mapear los corners de la celda de transición según la cara.
    // El siguiente es un ejemplo genérico:

    // Para cada corner de la celda de transición (0..7)
    for (int i = 0; i < 8; ++i) {
        // Calcula la posición del corner i en espacio mundo
        XMFLOAT3 cornerPos = ComputeTransitionCornerPosition(face, i, origin, size);
        // Consulta la densidad en esa posición (puede ser del nodo o del vecino)

        densities[i] = GetDensityAtWorldPos(cornerPos, node, voxelData, origin, size, world);
    }
}

// Función auxiliar: obtiene los extremos y densidades de una arista de transición
void MarchingCubes::GetTransitionEdgeVerts(
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
    // NOTA: Debes adaptar el mapeo de edges y corners a tu sistema y convención Transvoxel.
    // Se asume que transitionVertexData[caseIndex][edge] da los índices de los corners conectados.
    // Aquí se usa un ejemplo genérico:

    // Consulta los índices de los corners conectados por este edge
    int cornerA = GetTransitionEdgeCornerA(face, edge); // Debes implementar
    int cornerB = GetTransitionEdgeCornerB(face, edge); // Debes implementar

    // Calcula la posición de cada corner
    p1 = ComputeTransitionCornerPosition(face, cornerA, origin, size);
    p2 = ComputeTransitionCornerPosition(face, cornerB, origin, size);

    // Consulta la densidad en cada posición
    d1 = GetDensityAtWorldPos(p1, node, voxelData, origin, size, world);
    d2 = GetDensityAtWorldPos(p2, node, voxelData, origin, size, world);
}

// Tabla estándar de mapeo de corners de celda de transición para cada cara (Transvoxel)
// [face][corner] = {dx, dy, dz} en espacio local (0 o 1)
static const int transitionCornerOffsets[6][8][3] = {
    // +X
    { {1,0,0},{1,0,1},{1,1,1},{1,1,0},{0,0,0},{0,0,1},{0,1,1},{0,1,0} },
    // -X
    { {0,0,0},{0,0,1},{0,1,1},{0,1,0},{1,0,0},{1,0,1},{1,1,1},{1,1,0} },
    // +Y
    { {0,1,0},{0,1,1},{1,1,1},{1,1,0},{0,0,0},{0,0,1},{1,0,1},{1,0,0} },
    // -Y
    { {0,0,0},{0,0,1},{1,0,1},{1,0,0},{0,1,0},{0,1,1},{1,1,1},{1,1,0} },
    // +Z
    { {0,0,1},{1,0,1},{1,1,1},{0,1,1},{0,0,0},{1,0,0},{1,1,0},{0,1,0} },
    // -Z
    { {0,0,0},{1,0,0},{1,1,0},{0,1,0},{0,0,1},{1,0,1},{1,1,1},{0,1,1} }
};
// transitionCornerOffsets[face][corner][3]:
// Para cada una de las 6 caras (+X, -X, +Y, -Y, +Z, -Z), se define el orden de las 8 esquinas del cubo local respecto a esa cara.
// El orden de las esquinas para cada cara sigue la convención de Transvoxel (ver: Transvoxel Appendix B, Fig. 4 y 5).
// Es fundamental que este orden coincida con el usado en las tablas de triangulación de Transvoxel para evitar artefactos en los bordes LOD.

// Tabla estándar de edges de transición: cada edge conecta dos corners
// [edge][2] = {cornerA, cornerB}
// Mapea cada una de las 13 aristas de un cubo de transición a los índices de las dos esquinas que conecta.
// Orden:
// 0-3: base (aristas en z=0)
// 4-7: top (aristas en z=1)
// 8-11: verticales
// 12: diagonal (solo para Transvoxel)
// Este orden sigue la convención de Transvoxel (ver: Transvoxel Appendix B, Fig. 5).
static const int transitionEdgeCorners[13][2] = {
    {0,1},{1,2},{2,3},{3,0}, // base
    {4,5},{5,6},{6,7},{7,4}, // top
    {0,4},{1,5},{2,6},{3,7}, // verticals
    {0,2} // diagonal (solo para Transvoxel)
};

int MarchingCubes::GetTransitionEdgeCornerA(int face, int edge)
{
    return transitionEdgeCorners[edge][0];
}
int MarchingCubes::GetTransitionEdgeCornerB(int face, int edge)
{
    return transitionEdgeCorners[edge][1];
}

// --- Debes implementar estas funciones auxiliares según tu sistema ---
// Calcula la posición de un corner de transición según la cara y el índice de corner
XMFLOAT3 MarchingCubes::ComputeTransitionCornerPosition(int face, int corner, const XMFLOAT3& origin, float size)
{
    // Obtiene el offset local del corner para la cara y corner dados
    int dx = transitionCornerOffsets[face][corner][0];
    int dy = transitionCornerOffsets[face][corner][1];
    int dz = transitionCornerOffsets[face][corner][2];
    // Asume que la celda de transición es de tamaño size/2 en la dirección perpendicular a la cara
    float step = size / 2.0f;
    XMFLOAT3 pos = origin;
    pos.x += dx * step;
    pos.y += dy * step;
    pos.z += dz * step;
    return pos;
}

// Consulta la densidad en una posición del mundo
// Ahora requiere origin y size del nodo raíz (área) para SVO traversal
float MarchingCubes::GetDensityAtWorldPos(const XMFLOAT3& pos, const SVO_Node* node, const VoxelData* voxelData, const XMFLOAT3& origin, float nodeSize, World* world)
{
    // Log detallado de consulta de densidad
    char logBuf[256];
    sprintf_s(logBuf, sizeof(logBuf), "[GetDensityAtWorldPos] pos=(%.3f,%.3f,%.3f) origin=(%.3f,%.3f,%.3f) nodeSize=%.3f\n", pos.x, pos.y, pos.z, origin.x, origin.y, origin.z, nodeSize);
    OutputDebugStringA(logBuf);
    if (pos.x >= origin.x && pos.x < origin.x + nodeSize &&
        pos.y >= origin.y && pos.y < origin.y + nodeSize &&
        pos.z >= origin.z && pos.z < origin.z + nodeSize) {
        // Consulta en el SVO de este área
        const float NODE_MIN_SIZE = 1.0f;
        const SVO_Node* current = node;
        XMFLOAT3 currentOrigin = origin;
        float currentSize = nodeSize;
        while (current && !current->IsLeaf() && currentSize > NODE_MIN_SIZE) {
            float half = currentSize / 2.0f;
            int childIndex = 0;
            if (pos.x >= currentOrigin.x + half) childIndex |= 1;
            if (pos.y >= currentOrigin.y + half) childIndex |= 2;
            if (pos.z >= currentOrigin.z + half) childIndex |= 4;
            if (!current->HasChild(childIndex)) break;
            currentOrigin.x += (childIndex & 1) ? half : 0.0f;
            currentOrigin.y += (childIndex & 2) ? half : 0.0f;
            currentOrigin.z += (childIndex & 4) ? half : 0.0f;
            currentSize = half;
            current = current->GetChild(childIndex);
        }
        float result = (current && current->IsOccupied()) ? 1.0f : 0.0f;
        // Log resultado local
        char resBuf[256];
        sprintf_s(resBuf, sizeof(resBuf), "[GetDensityAtWorldPos]   -> LOCAL result=%.1f (leaf=%d, occ=%d)\n", result, current ? current->IsLeaf() : -1, current ? current->IsOccupied() : -1);
        OutputDebugStringA(resBuf);
        return result;
    } else if (world) {
        AreaKey key = world->GetAreaKeyFromPosition(pos);
        SVO_Node* neighborRoot = world->GetOrCreateArea(key);
        XMFLOAT3 neighborOrigin = { static_cast<float>(key.x) * AREA_SIZE_F, static_cast<float>(key.y) * AREA_SIZE_F, static_cast<float>(key.z) * AREA_SIZE_F };
        // Log salto a área vecina
        char areaBuf[256];
        sprintf_s(areaBuf, sizeof(areaBuf), "[GetDensityAtWorldPos]   -> NEIGHBOR areaKey=(%d,%d,%d) neighborOrigin=(%.3f,%.3f,%.3f)\n", key.x, key.y, key.z, neighborOrigin.x, neighborOrigin.y, neighborOrigin.z);
        OutputDebugStringA(areaBuf);
        return GetDensityAtWorldPos(pos, neighborRoot, voxelData, neighborOrigin, AREA_SIZE_F, world);
    } else {
        // Log vacío
        OutputDebugStringA("[GetDensityAtWorldPos]   -> OUT OF BOUNDS, returns 0.0\n");
        return 0.0f;
    }
}