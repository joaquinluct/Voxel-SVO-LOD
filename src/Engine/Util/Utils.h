#pragma once

#include <cmath>
#include <directxmath.h>

using namespace DirectX;

template <typename T>
void SafeRelease(T*& ptr) {
    if (ptr) {
        ptr->Release();  // Llama a Release si el puntero no es nulo
        ptr = nullptr;   // Establece el puntero a nullptr
    }
};

template <typename T>
void SafeShutDown(T*& ptr) {
    if (ptr) {
        ptr->Shutdown();  // Llama a Release si el puntero no es nulo
        ptr = nullptr;   // Establece el puntero a nullptr
    }

};

template <typename T>
void SafeDelete(T*& ptr) {
    if (ptr) {
        delete ptr;      // Libera la memoria del objeto
        ptr = nullptr;   // Establece el puntero a nullptr
    }
};

struct Vertex {
    DirectX::XMFLOAT3 Position; // Posici�n del v�rtice
    DirectX::XMFLOAT4 Color;    // Color del v�rtice
};

// Estructura para un v�rtice de Marching Cubes
//struct MarchingCubesVertex {
//    XMFLOAT3 Position;
//    XMFLOAT3 Normal;
//};

//struct MarchingCubesVertex {
//    DirectX::XMFLOAT3 Position; // Posici�n del v�rtice
//    DirectX::XMFLOAT4 Color;    // Color del v�rtice
//};

// Estructura de v�rtice para Marching Cubes
// Debe ser visible globalmente, por eso se define aqu�.
const float EPSILON = 0.00001f;
struct MarchingCubesVertex {
    XMFLOAT3 Position;
    XMFLOAT3 Normal; // Opcional, pero se calcula en MarchingCubes.cpp
    XMFLOAT4 Color;  // A�adido para el color del v�rtice

    // Operadores de comparaci�n para std::map (para detectar v�rtices �nicos)
    bool operator<(const MarchingCubesVertex& other) const {
        if (Position.x != other.Position.x) return Position.x < other.Position.x;
        if (Position.y != other.Position.y) return Position.y < other.Position.y;
        return Position.z < other.Position.z;
        /*if (Position.z != other.Position.z) return Position.z < other.Position.z;
        if (Normal.x != other.Normal.x) return Normal.x < other.Normal.x;
        if (Normal.y != other.Normal.y) return Normal.y < other.Normal.y;
        if (Normal.z != other.Normal.z) return Normal.z < other.Normal.z;
        if (Color.x != other.Color.x) return Color.x < other.Color.x;
        if (Color.y != other.Color.y) return Color.y < other.Color.y;
        if (Color.z != other.Color.z) return Color.z < other.Color.z;
        return Color.w < other.Color.w;*/
    }
    bool operator==(const MarchingCubesVertex& other) const {
        return std::fabs(Position.x - other.Position.x) < EPSILON &&
            std::fabs(Position.y - other.Position.y) < EPSILON &&
            std::fabs(Position.z - other.Position.z) < EPSILON;
        /*std::fabs(Color.x - other.Color.x) < EPSILON &&
        std::fabs(Color.y - other.Color.y) < EPSILON &&
        std::fabs(Color.z - other.Color.z) < EPSILON &&
        std::fabs(Color.w - other.Color.w) < EPSILON &&
        std::fabs(Normal.x - other.Normal.x) < EPSILON &&
        std::fabs(Normal.y - other.Normal.y) < EPSILON &&
        std::fabs(Normal.z - other.Normal.z) < EPSILON;*/
    }
};
namespace std {
    template<> struct hash<MarchingCubesVertex> {
        size_t operator()(const MarchingCubesVertex& v) const {
            // Función auxiliar para combinar hashes de forma robusta
            // Adaptado de la idea de Boost's hash_combine
            size_t h = 0;
            auto hash_combine = [&](size_t& seed, float val) {
                seed ^= std::hash<float>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                };

            // Hashing con cuantificación para manejar la tolerancia en floats
            // Puedes ajustar el 'quantizeStep' si EPSILON es muy pequeño
            const float QUANTIZE_STEP = EPSILON * 100.0f; // Un paso de cuantificación más grande que EPSILON

            hash_combine(h, std::round(v.Position.x / QUANTIZE_STEP) * QUANTIZE_STEP);
            hash_combine(h, std::round(v.Position.y / QUANTIZE_STEP) * QUANTIZE_STEP);
            hash_combine(h, std::round(v.Position.z / QUANTIZE_STEP) * QUANTIZE_STEP);
            hash_combine(h, std::round(v.Normal.x / QUANTIZE_STEP) * QUANTIZE_STEP);
            hash_combine(h, std::round(v.Normal.y / QUANTIZE_STEP) * QUANTIZE_STEP);
            hash_combine(h, std::round(v.Normal.z / QUANTIZE_STEP) * QUANTIZE_STEP);
            hash_combine(h, std::round(v.Color.x / QUANTIZE_STEP) * QUANTIZE_STEP);
            hash_combine(h, std::round(v.Color.y / QUANTIZE_STEP) * QUANTIZE_STEP);
            hash_combine(h, std::round(v.Color.z / QUANTIZE_STEP) * QUANTIZE_STEP);
            hash_combine(h, std::round(v.Color.w / QUANTIZE_STEP) * QUANTIZE_STEP);

            return h;
        }
    };
} // namespace std


//// Estructur para la luz
//cbuffer LightBuffer : register(b1)
//{
//    float3 lightDirection; // Direcci�n de la luz (normalizada)
//    float4 diffuseColor;   // Color de la luz difusa
//    float4 ambientColor;   // Color de la luz ambiental
//    float4 specularColor;  // Color de la luz especular
//    float specularPower;   // Potencia especular
//};
