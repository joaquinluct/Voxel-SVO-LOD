#include <DirectXMath.h>
#include <cmath> // Para std::fabs

// Definir una tolerancia pequeña para comparaciones de punto flotante
const float EPSILON = 0.0001f; // O un valor más apropiado para tu aplicación

static bool AreEqual(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
    return std::fabs(v1.x - v2.x) < EPSILON &&
        std::fabs(v1.y - v2.y) < EPSILON &&
        std::fabs(v1.z - v2.z) < EPSILON;
}