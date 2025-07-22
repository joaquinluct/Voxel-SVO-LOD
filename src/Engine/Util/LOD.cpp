#include <DirectXMath.h>
#include <algorithm>

using namespace DirectX;

static float DistanceToAABB(const XMFLOAT3& point, const XMFLOAT3& min, const XMFLOAT3& max) {
    float dx = std::max({ min.x - point.x, 0.0f, point.x - max.x });
    float dy = std::max({ min.y - point.y, 0.0f, point.y - max.y });
    float dz = std::max({ min.z - point.z, 0.0f, point.z - max.z });
    return static_cast<float>(sqrt(static_cast<double>(dx * dx + dy * dy + dz * dz)));
}