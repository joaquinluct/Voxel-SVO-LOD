#include "RayTracing.h"
#include <limits>

namespace Util {

    RayTracing::RayTracing() {}

    RayTracing::~RayTracing() {}

    float RayTracing::Trace(const XMFLOAT3& origin, const XMFLOAT3& direction, float maxDistance, const std::vector<Triangle>& triangles) const {
        float closestDistance = maxDistance; // Inicializar con la distancia máxima
        bool intersectionFound = false;

        for (const auto& triangle : triangles) {
            float t = 0.0f;
            if (IntersectRayTriangle(origin, direction, triangle, t) && t > 0.0f && t < closestDistance) {
                closestDistance = t;
                intersectionFound = true;
            }
        }

        if (intersectionFound) {
            return closestDistance;
        }
        else {
            return -1.0f; // No hay intersección
        }
    }

    float RayTracing::Trace(const XMFLOAT3& origin, const XMFLOAT3& target, const std::vector<Triangle>& triangles) const {
        // Calcular la dirección del rayo
        XMFLOAT3 direction = {
            target.x - origin.x,
            target.y - origin.y,
            target.z - origin.z
        };

        // Calcular la longitud del rayo
        float rayLength = Distance(origin, target);

        // Normalizar la dirección del rayo
        XMVECTOR rayDirection = XMVector3Normalize(XMLoadFloat3(&direction));
        XMStoreFloat3(&direction, rayDirection);

        return Trace(origin, direction, rayLength, triangles);
    }

    float RayTracing::Distance(const XMFLOAT3& p1, const XMFLOAT3& p2) const {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float dz = p2.z - p1.z;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    bool RayTracing::IntersectRayTriangle(const XMFLOAT3& origin, const XMFLOAT3& direction, const Triangle& triangle, float& t) const {
        const float EPSILON = 0.000001f;
        XMFLOAT3 edge1 = { triangle.v1.x - triangle.v0.x, triangle.v1.y - triangle.v0.y, triangle.v1.z - triangle.v0.z };
        XMFLOAT3 edge2 = { triangle.v2.x - triangle.v0.x, triangle.v2.y - triangle.v0.y, triangle.v2.z - triangle.v0.z };

        XMVECTOR pvec = XMVector3Cross(XMLoadFloat3(&direction), XMLoadFloat3(&edge2));
        float det = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&edge1), pvec));

        if (det > -EPSILON && det < EPSILON)
            return false;

        float inv_det = 1.0f / det;

        XMFLOAT3 tvec = { origin.x - triangle.v0.x, origin.y - triangle.v0.y, origin.z - triangle.v0.z };
        float u = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&tvec), pvec)) * inv_det;
        if (u < 0.0f || u > 1.0f)
            return false;

        XMVECTOR qvec = XMVector3Cross(XMLoadFloat3(&tvec), XMLoadFloat3(&edge1));
        float v = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&direction), qvec)) * inv_det;
        if (v < 0.0f || u + v > 1.0f)
            return false;

        t = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&edge2), qvec)) * inv_det;

        return true;
    }

    float RayTracing::Trace(const XMFLOAT3& origin, const XMFLOAT3& direction, float maxDistance, const BoundingBox& aabb) const {
        float tmin = 0.0f;
        float tmax = maxDistance;

        for (int i = 0; i < 3; ++i) {
            float t1 = ((&aabb.min.x)[i] - (&origin.x)[i]) / (&direction.x)[i];
            float t2 = ((&aabb.max.x)[i] - (&origin.x)[i]) / (&direction.x)[i];

            if (t1 > t2) std::swap(t1, t2);

            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);

            if (tmin > tmax) return -1.0f;
        }

        return tmin;
    }

    bool RayTracing::Trace(const XMFLOAT3& origin, const XMFLOAT3& direction, float maxDistance, const BoundingBox& aabb, XMFLOAT3& intersectionPoint) const {
        float t = Trace(origin, direction, maxDistance, aabb);

        if (t > 0.0f) {
            // Calcular el punto de intersección
            intersectionPoint.x = origin.x + direction.x * t;
            intersectionPoint.y = origin.y + direction.y * t;
            intersectionPoint.z = origin.z + direction.z * t;
            return true;
        }

        return false;
    }

    bool RayTracing::IntersectRayAABB(const XMFLOAT3& origin, const XMFLOAT3& direction, float maxDistance, const BoundingBox& aabb, float& t) const {
        float tmin = 0.0f;
        float tmax = maxDistance;

        for (int i = 0; i < 3; ++i) {
            float invD = 1.0f / (&direction.x)[i];
            float t0 = ((&aabb.min.x)[i] - (&origin.x)[i]) * invD;
            float t1 = ((&aabb.max.x)[i] - (&origin.x)[i]) * invD;

            if (invD < 0.0f) {
                std::swap(t0, t1);
            }

            tmin = std::max(tmin, t0);
            tmax = std::min(tmax, t1);

            if (tmax <= tmin) {
                return false;
            }
        }

        t = tmin;
        return true;
    }

} // namespace Util