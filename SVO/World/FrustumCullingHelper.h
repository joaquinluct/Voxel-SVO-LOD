#pragma once
#include <DirectXMath.h>

namespace FrustumCullingHelper {
    // Comprueba si una esfera está dentro del frustum
    // params:
    //   center: Centro de la esfera
    //   radius: Radio de la esfera
    //   planes: Los 6 planos del frustum (left, right, top, bottom, near, far)
    // returns: true si la esfera está total o parcialmente dentro del frustum
    inline bool IsSphereInFrustum(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4 planes[6]) {
        for (int i = 0; i < 6; ++i) {
            // Calcular la distancia desde el centro de la esfera al plano
            float distance = planes[i].x * center.x + 
                             planes[i].y * center.y + 
                             planes[i].z * center.z + 
                             planes[i].w;
            
            // Si la distancia es menor que -radio, la esfera está completamente fuera del frustum
            if (distance < -radius) {
                return false;
            }
        }
        // La esfera está al menos parcialmente dentro del frustum
        return true;
    }

    // Comprueba si un cubo o caja AABB está dentro del frustum
    // params:
    //   min: Esquina mínima del cubo (esquina inferior izquierda trasera)
    //   max: Esquina máxima del cubo (esquina superior derecha delantera)
    //   planes: Los 6 planos del frustum (left, right, top, bottom, near, far)
    // returns: true si el cubo está total o parcialmente dentro del frustum
    inline bool IsAABBInFrustum(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max, const DirectX::XMFLOAT4 planes[6]) {
        for (int i = 0; i < 6; ++i) {
            // Encontrar el punto más lejano del cubo en la dirección de la normal del plano
            DirectX::XMFLOAT3 p;
            p.x = (planes[i].x > 0) ? max.x : min.x;
            p.y = (planes[i].y > 0) ? max.y : min.y;
            p.z = (planes[i].z > 0) ? max.z : min.z;
            
            // Si este punto está fuera del plano, todo el cubo está fuera
            float distance = planes[i].x * p.x + 
                             planes[i].y * p.y + 
                             planes[i].z * p.z + 
                             planes[i].w;
            
            if (distance < 0) {
                return false;
            }
        }
        // El cubo está al menos parcialmente dentro del frustum
        return true;
    }
}
