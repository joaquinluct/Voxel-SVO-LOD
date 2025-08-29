#pragma once
#include <DirectXMath.h>
#include <Defines/CameraDefinition.h>

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

    inline float GetPlaneCoeff(const CameraDefinition::FrustumPlane& p, int index) {
        switch (index) {
        case 0: return p.coefficients.x;
        case 1: return p.coefficients.y;
        case 2: return p.coefficients.z;
        case 3: return p.coefficients.w;
        }
        return 0.0f;
    }

    inline bool IsAABBInFrustum(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max, const std::vector<CameraDefinition::FrustumPlane>& planes) {

        // El bucle ahora itera sobre los elementos del vector
        for (const auto& p : planes) {

            // Puntos de la AABB que se usarán para la prueba
            DirectX::XMFLOAT3 pVertex, nVertex;

            // Determinar el vértice "positivo" (P-vertex) y el "negativo" (N-vertex)
            // para este plano.
            if (GetPlaneCoeff(p, 0) >= 0) {
                pVertex.x = max.x;
                nVertex.x = min.x;
            }
            else {
                pVertex.x = min.x;
                nVertex.x = max.x;
            }
            if (GetPlaneCoeff(p, 1) >= 0) {
                pVertex.y = max.y;
                nVertex.y = min.y;
            }
            else {
                pVertex.y = min.y;
                nVertex.y = max.y;
            }
            if (GetPlaneCoeff(p, 2) >= 0) {
                pVertex.z = max.z;
                nVertex.z = min.z;
            }
            else {
                pVertex.z = min.z;
                nVertex.z = max.z;
            }

            // Si el P-vertex está fuera del plano, la AABB está completamente fuera.
            // Distancia del P-vertex al plano: (Ax*Px + By*Py + Cz*Pz + D)
            if ((GetPlaneCoeff(p, 0) * pVertex.x + GetPlaneCoeff(p, 1) * pVertex.y + GetPlaneCoeff(p, 2) * pVertex.z + GetPlaneCoeff(p, 3)) < 0) {
                return false;
            }

            // Si el N-vertex está fuera, la AABB cruza el plano
            // Distancia del N-vertex al plano: (Ax*Nx + By*Ny + Cz*Nz + D)
            // (Comentario opcional: Esto se puede usar para determinar si la AABB intersecta o está contenida,
            // pero para un simple culling de "dentro/fuera" no es estrictamente necesario si ya tenemos la primera prueba)
            if ((GetPlaneCoeff(p, 0) * nVertex.x + GetPlaneCoeff(p, 1) * nVertex.y + GetPlaneCoeff(p, 2) * nVertex.z + GetPlaneCoeff(p, 3)) < 0) {
                // El N-vertex está fuera, pero el P-vertex no, por lo que intersecta el plano.
            }
        }

        return true; // La AABB está dentro del frustum
    }
}
