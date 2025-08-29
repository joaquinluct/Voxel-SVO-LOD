#pragma once

#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

namespace Util {

    struct Triangle {
        XMFLOAT3 v0;
        XMFLOAT3 v1;
        XMFLOAT3 v2;
    };

    struct BoundingBox {
        XMFLOAT3 min;
        XMFLOAT3 max;
        DirectX::XMFLOAT3 GetCenter() const {
            return XMFLOAT3((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f, (min.z + max.z) / 2.0f);
		}
    };

    class RayTracing {
    public:
        RayTracing();
        ~RayTracing();

        // Traza un rayo desde 'origin' hasta 'target' y devuelve la distancia.
        // Si no hay intersección, devuelve un valor negativo.
        float Trace(const XMFLOAT3& origin, const XMFLOAT3& target, const std::vector<Triangle>& triangles) const;

        // Traza un rayo desde 'origin' en la dirección 'direction' y devuelve la distancia.
        // Si no hay intersección, devuelve un valor negativo.
        float Trace(const XMFLOAT3& origin, const XMFLOAT3& direction, float maxDistance, const std::vector<Triangle>& triangles) const;

        // Traza un rayo contra un bounding box y devuelve la distancia a la intersección.
        // Si no hay intersección, devuelve un valor negativo.
        float Trace(const XMFLOAT3& origin, const XMFLOAT3& direction, float maxDistance, const BoundingBox& aabb) const;

        // Traza un rayo contra un bounding box y devuelve el punto de intersección.
        // Si no hay intersección, devuelve false.
        bool Trace(const XMFLOAT3& origin, const XMFLOAT3& direction, float maxDistance, const BoundingBox& aabb, XMFLOAT3& intersectionPoint) const;

        float Distance(const XMFLOAT3& p1, const XMFLOAT3& p2) const;
    private:
        // Métodos auxiliares (puedes añadir más según sea necesario)
        bool IntersectRayTriangle(const XMFLOAT3& origin, const XMFLOAT3& direction, const Triangle& triangle, float& t) const;
        bool IntersectRayAABB(const XMFLOAT3& origin, const XMFLOAT3& direction, float maxDistance, const BoundingBox& aabb, float& t) const;
    };

} // namespace Util