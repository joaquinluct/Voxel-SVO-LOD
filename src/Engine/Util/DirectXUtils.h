#pragma once
#define NOMINMAX
#include <cmath> // Para std::fabs
#include <DirectXMath.h>
#include <string>
#include <vector>

using namespace DirectX;

namespace DirectXUtils
{
    XMFLOAT3 Add(const XMFLOAT3& v1, const XMFLOAT3& v2);
    XMFLOAT3 AddInt(const XMFLOAT3& v1, const int v2[3]);
    bool AreEqual(const XMFLOAT3& v1, const XMFLOAT3& v2);
    XMFLOAT3 Subtract(const XMFLOAT3& a, const XMFLOAT3& b);
    XMFLOAT3 Multiply(const XMFLOAT3& a, float scalar);
    XMFLOAT4 Multiply(const XMFLOAT4& a, float scalar);
    XMFLOAT3 Divide(const XMFLOAT3& a, float scalar);
    XMFLOAT3 Normalize(const XMFLOAT3& v);
    XMFLOAT3 Cross(const XMFLOAT3& v1, const XMFLOAT3& v2);
    XMFLOAT3 Lerp(const XMFLOAT3& v1, const XMFLOAT3& v2, float t);
    XMFLOAT3 Transform(const XMFLOAT3& v, const XMMATRIX& m);
    bool IsNormalPointing(const XMFLOAT3& normal, const XMFLOAT3& direction, float dotThreshold = 0.999f);
    float Dot(const XMFLOAT3& a, const XMFLOAT3& b);
    float Smoothstep(float edge0, float edge1, float x);
    // Calcula la distancia euclidiana entre dos vectores
    float Distance(const XMFLOAT3& v1, const XMFLOAT3& v2);
    // Calcula la distancia euclidiana al cuadrado entre dos vectores
    // Útil para comparaciones de distancia sin la raíz cuadrada
    float DistanceSq(const XMFLOAT3& v1, const XMFLOAT3& v2);
    XMFLOAT3 ToXMFLOAT3(const std::vector<float>& vec);
    XMFLOAT4 ToXMFLOAT4(const std::vector<float>& vec);

    // COLOR
    DirectX::XMFLOAT4 GenerateRandomColor();

    inline static float Saturate(float value) {
        float max = std::fmax(value, 0.0f);
        float min = std::fmin(max, 1.0f);
        return min;
    }

    DirectX::XMFLOAT4 SaturateColor(const DirectX::XMFLOAT4& color);

    std::string ToString(const XMFLOAT3& v);

    // Constantes de direcciones cardinales (útiles para IsNormalPointing)
    // Asegúrate de que tus ejes XYZ coincidan con estas convenciones.
    // Asumo Y+ = Arriba, Z- = Norte, X+ = Este.
    static const XMFLOAT3 UP_DIR = { 0.0f, 1.0f, 0.0f };
    static const XMFLOAT3 DOWN_DIR = { 0.0f, -1.0f, 0.0f };
    static const XMFLOAT3 NORTH_DIR = { 0.0f, 0.0f, -1.0f }; // Si tu 'z' crece hacia el "sur"
    static const XMFLOAT3 SOUTH_DIR = { 0.0f, 0.0f, 1.0f };  // Si tu 'z' crece hacia el "sur"
    static const XMFLOAT3 EAST_DIR = { 1.0f, 0.0f, 0.0f };
    static const XMFLOAT3 WEST_DIR = { -1.0f, 0.0f, 0.0f };
}
