#include "MarchingCubesUtil.h"
#include "../World/World.h"
#include "MarchingCubesTables.h"
#include <algorithm>     // Para std::min, std::max, std::clamp si se usan
#include <cmath>         // Para std::fmod, std::abs si se usan

namespace MarchingCubesUtil {

float MarchingCubesUtil::GetDensityAtWorldPos(const DirectX::XMFLOAT3& pos, const SVO_Node* node, const VoxelData* voxelData, const DirectX::XMFLOAT3& origin, float nodeSize, World* world)
{
     const float CHUNK_SIZE = 1024.0f; 

    DirectX::XMFLOAT3 localPos = {
        std::fmod(pos.x, CHUNK_SIZE),
        std::fmod(pos.y, CHUNK_SIZE),
        std::fmod(pos.z, CHUNK_SIZE)
    };
    if (localPos.x < 0) localPos.x += CHUNK_SIZE;
    if (localPos.y < 0) localPos.y += CHUNK_SIZE;
    if (localPos.z < 0) localPos.z += CHUNK_SIZE;

    return world->SphereSDF(localPos, XMFLOAT3{ 120.0f,120.0f,120.0f }, 60.0f);

    //const float boxMin = 30.0f; 
    //const float boxMax = 80.0f;
    //// La función ya incluye smoothnessRadius internamente

    //return world->CubeSDF(localPos, {boxMin, boxMin, boxMin}, {boxMax, boxMax, boxMax});

    //float boxInit = 20.0f;
    //float boxEnd = 80.0f;

    //// Un punto se considera "material" (densidad 0.0f) si está dentro del volumen del cubo.
    //// De lo contrario, se considera "vacío" (densidad 1.0f).
    //bool isInsideSolidCube = (pos.x-nodeSize >= boxInit && pos.x <= boxEnd+nodeSize &&
    //    pos.y-nodeSize >= boxInit && pos.y <= boxEnd+nodeSize &&
    //    pos.z-nodeSize >= boxInit && pos.z <= boxEnd+nodeSize);

    //if (isInsideSolidCube) {
    //    return 0.0f; // Dentro del cubo: material
    //}
    //else {
    //    return 1.0f; // Fuera del cubo: vacío
    //}
    
    
    // 2. Obtener la densidad precisa del mundo.
    //    Aquí delegamos a la clase 'World' para que defina la densidad.
    //    Pasamos la posición global 'pos' y el 'currentSVO_Size' (el tamaño del vóxel en este LOD).
    float density = 1.0f;
    if (world) {
        if (DirectXUtils::AreEqual(origin, pos)) {
            density = node->GetDensity();
        } else {
            density = world->GetVoxelDensity(pos, node, nodeSize);        
        }
    }    
    if (density < 0.999999f && density > 0.0000001f) {
        density = density;
    }
    if (density < 0.0000001f) {
        density = density;
    }
    return density;
}
    

float fillCube(XMFLOAT3 const pos) {
    float boxInit = 86.0f;
    float boxEnd = 212.0f;

    // Un punto se considera "material" (densidad 0.0f) si está dentro del volumen del cubo.
    // De lo contrario, se considera "vacío" (densidad 1.0f).
    bool isInsideSolidCube = (pos.x >= boxInit && pos.x < boxEnd &&
        pos.y >= boxInit && pos.y < boxEnd &&
        pos.z >= boxInit && pos.z < boxEnd);

    if (isInsideSolidCube) {
        return 0.0f; // Dentro del cubo: material
    }
    else {
        return 1.0f; // Fuera del cubo: vacío
    }
}

// Asegurando que CalculateNormal esté dentro del namespace MarchingCubesUtil
XMFLOAT3 MarchingCubesUtil::CalculateNormal(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3) {
    XMVECTOR v1 = XMLoadFloat3(&p1);
    XMVECTOR v2 = XMLoadFloat3(&p2);
    XMVECTOR v3 = XMLoadFloat3(&p3);
    XMVECTOR n = XMVector3Cross(v2 - v1, v3 - v1);
    n = XMVector3Normalize(n);
    XMFLOAT3 normal;
    XMStoreFloat3(&normal, n);
    return normal;
}

XMFLOAT3 InterpolateVerts2(XMFLOAT3 p1, XMFLOAT3 p2, float valp1, float valp2, int ISO_LEVEL) { //
    //if (abs(valp2 - valp1) < 0.00001f) { // Evita divisi�n por cero y maneja casos de misma densidad
    //	return p1; // Devuelve el primer punto si las densidades son casi iguales
    //}
    float mu = (ISO_LEVEL - valp1) / (valp2 - valp1);
    // Asegúrate de que mu esté en el rango [0, 1]
    mu = (mu < 0.0f) ? 0.0f : (mu > 1.0f ? 1.0f : mu);
    return XMFLOAT3(p1.x + mu * (p2.x - p1.x), p1.y + mu * (p2.y - p1.y), p1.z + mu * (p2.z - p1.z)); //
}

XMFLOAT3 InterpolateVerts(XMFLOAT3 p1, XMFLOAT3 p2, float valp1, float valp2, int ISO_LEVEL) {
    // Evita división por cero y maneja casos de misma densidad
    if (std::abs(valp2 - valp1) < 1e-6f) {
        return p1; // Devuelve el primer punto si las densidades son casi iguales
    }
    // Interpolación lineal
	if (valp1 == valp2) return p1;
    float t = (ISO_LEVEL - valp1) / (valp2 - valp1);
    return XMFLOAT3(
        p1.x + t * (p2.x - p1.x),
        p1.y + t * (p2.y - p1.y),
        p1.z + t * (p2.z - p1.z)
    );
}

//DirectX::XMFLOAT3 InterpolateVerts(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, float valp1, float valp2, int ISO_LEVEL) {
//
//    // Carga los XMFLOAT3 a XMVECTOR para operaciones SIMD
//    DirectX::XMVECTOR vecP1 = DirectX::XMLoadFloat3(&p1);
//    DirectX::XMVECTOR vecP2 = DirectX::XMLoadFloat3(&p2);
//
//    float densityDiff = valp2 - valp1;
//
//    // Usa una función IsNear para mayor robustez en la comparación de punto flotante
//    // Si no tienes DirectXUtils::IsNear, puedes usar std::abs(densityDiff) < 1e-6f
//    if (std::abs(densityDiff) < 1e-6f) { // O DirectXUtils::IsNear(valp1, valp2)
//        return p1; // Devuelve el primer punto si las densidades son casi iguales
//    }
//
//    float t = (static_cast<float>(ISO_LEVEL) - valp1) / densityDiff;
//
//    // Crea un XMVECTOR con 't' replicado en todas las componentes
//    DirectX::XMVECTOR vecT = DirectX::XMVectorReplicate(t);
//
//    // Realiza la interpolación lineal usando operaciones vectoriales
//    // result = vecP1 + vecT * (vecP2 - vecP1)
//    DirectX::XMVECTOR resultVec = DirectX::XMVectorAdd(vecP1, DirectX::XMVectorMultiply(vecT, DirectX::XMVectorSubtract(vecP2, vecP1)));
//
//    // Almacena el resultado de vuelta en un XMFLOAT3
//    DirectX::XMFLOAT3 result;
//    DirectX::XMStoreFloat3(&result, resultVec);
//    return result;
//}

XMFLOAT3 CalculateDensityNormal(const XMFLOAT3& pos, const SVO_Node* node, const VoxelData* voxelData, const XMFLOAT3& origin, float nodeSize, World* world) {
    const float h = 0.5f;
    //const float h = nodeSize / 2;
    float dx = MarchingCubesUtil::GetDensityAtWorldPos({ pos.x + h, pos.y, pos.z }, node, voxelData, origin, nodeSize, world)
        - MarchingCubesUtil::GetDensityAtWorldPos({ pos.x - h, pos.y, pos.z }, node, voxelData, origin, nodeSize, world);
    float dy = MarchingCubesUtil::GetDensityAtWorldPos({ pos.x, pos.y + h, pos.z }, node, voxelData, origin, nodeSize, world)
        - MarchingCubesUtil::GetDensityAtWorldPos({ pos.x, pos.y - h, pos.z }, node, voxelData, origin, nodeSize, world);
    float dz = MarchingCubesUtil::GetDensityAtWorldPos({ pos.x, pos.y, pos.z + h }, node, voxelData, origin, nodeSize, world)
        - MarchingCubesUtil::GetDensityAtWorldPos({ pos.x, pos.y, pos.z - h }, node, voxelData, origin, nodeSize, world);

    XMVECTOR grad = XMVectorSet(dx, dy, dz, 0.0f);
    return DirectXUtils::Normalize(XMFLOAT3(dx, dy, dz));
    float len = XMVectorGetX(XMVector3Length(grad));
    XMFLOAT3 normal;
    //if (len < 1e-4f) {
    //    // Normal nula: forzar normal hacia fuera según la frontera del cubo [10,100]^3
    //    float dist_x_min = std::abs(pos.x - 10.0f);
    //    float dist_x_max = std::abs(pos.x - 100.0f);
    //    float dist_y_min = std::abs(pos.y - 10.0f);
    //    float dist_y_max = std::abs(pos.y - 100.0f);
    //    float dist_z_min = std::abs(pos.z - 10.0f);
    //    float dist_z_max = std::abs(pos.z - 100.0f);
    //    float min_dist = dist_x_min;
    //    normal = {-1,0,0};
    //    if (dist_x_max < min_dist) { min_dist = dist_x_max; normal = {1,0,0}; }
    //    if (dist_y_min < min_dist) { min_dist = dist_y_min; normal = {0,-1,0}; }
    //    if (dist_y_max < min_dist) { min_dist = dist_y_max; normal = {0,1,0}; }
    //    if (dist_z_min < min_dist) { min_dist = dist_z_min; normal = {0,0,-1}; }
    //    if (dist_z_max < min_dist) { min_dist = dist_z_max; normal = {0,0,1}; }
    //} else {
    grad = XMVector3Normalize(grad);
    XMStoreFloat3(&normal, grad);
    //}
    return normal;
}

int GetCubeIndex(float(&cube)[8], XMFLOAT3 origin, float size, const SVO_Node* node, const VoxelData* voxelData, World* world, float ISO_LEVEL)
{    
    for (int i = 0; i < 8; ++i) {
        XMFLOAT3 cornerPos = DirectXUtils::Add(origin, DirectXUtils::Multiply(MarchingCubesTables::cornerOffsetsX[i], size));
        cube[i] = MarchingCubesUtil::GetDensityAtWorldPos(cornerPos, node, voxelData, origin, size, world);
        // cube[i] = world->GetVoxelDensity(origin);
    }    
    // Calcula el índice de la tabla
    int cubeIndex = 0;
    for (int i = 0; i < 8; ++i) {
        if (cube[i] >= ISO_LEVEL) cubeIndex |= (1 << i);
        // if (cube[i] < ISO_LEVEL) cubeIndex |= (1 << i);
    }    
    return cubeIndex;
}

void InterpolateEdgeVerts(int cubeIndex, const XMFLOAT3 p_local[8], const float density[8], XMFLOAT3 (&vertlist)[12], float ISO_LEVEL) {
    using namespace MarchingCubesTables;
    
    // Encontrar los puntos de intersección de la superficie con las aristas del cubo
    // Y almacenar las posiciones interpoladas en vertlist
    if (edgeTable[cubeIndex] & 0x001) vertlist[0] = InterpolateVerts(p_local[0], p_local[1], density[0], density[1], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x002) vertlist[1] = InterpolateVerts(p_local[1], p_local[2], density[1], density[2], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x004) vertlist[2] = InterpolateVerts(p_local[2], p_local[3], density[2], density[3], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x008) vertlist[3] = InterpolateVerts(p_local[3], p_local[0], density[3], density[0], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x010) vertlist[4] = InterpolateVerts(p_local[4], p_local[5], density[4], density[5], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x020) vertlist[5] = InterpolateVerts(p_local[5], p_local[6], density[5], density[6], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x040) vertlist[6] = InterpolateVerts(p_local[6], p_local[7], density[6], density[7], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x080) vertlist[7] = InterpolateVerts(p_local[7], p_local[4], density[7], density[4], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x100) vertlist[8] = InterpolateVerts(p_local[0], p_local[4], density[0], density[4], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x200) vertlist[9] = InterpolateVerts(p_local[1], p_local[5], density[1], density[5], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x400) vertlist[10] = InterpolateVerts(p_local[2], p_local[6], density[2], density[6], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x800) vertlist[11] = InterpolateVerts(p_local[3], p_local[7], density[3], density[7], ISO_LEVEL);

    /*if (edgeTable[cubeIndex] & 0x001) vertlist[0] = InterpolateVerts(p_local[0], p_local[1], density[0], density[1], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x002) vertlist[1] = InterpolateVerts(p_local[1], p_local[3], density[1], density[3], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x004) vertlist[3] = InterpolateVerts(p_local[3], p_local[2], density[3], density[2], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x008) vertlist[2] = InterpolateVerts(p_local[2], p_local[0], density[2], density[0], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x010) vertlist[4] = InterpolateVerts(p_local[4], p_local[5], density[4], density[5], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x020) vertlist[5] = InterpolateVerts(p_local[5], p_local[6], density[5], density[6], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x040) vertlist[6] = InterpolateVerts(p_local[6], p_local[7], density[6], density[7], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x080) vertlist[7] = InterpolateVerts(p_local[7], p_local[4], density[7], density[4], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x100) vertlist[8] = InterpolateVerts(p_local[0], p_local[4], density[0], density[4], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x200) vertlist[9] = InterpolateVerts(p_local[1], p_local[5], density[1], density[5], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x400) vertlist[10] = InterpolateVerts(p_local[2], p_local[6], density[2], density[6], ISO_LEVEL);
    if (edgeTable[cubeIndex] & 0x800) vertlist[11] = InterpolateVerts(p_local[3], p_local[7], density[3], density[7], ISO_LEVEL);*/
}

} // namespace MarchingCubesUtil
