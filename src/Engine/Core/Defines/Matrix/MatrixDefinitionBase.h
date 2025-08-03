#pragma once

#include <DirectXMath.h>
#include <string_view>

constexpr std::string_view MATRIX_TYPE_VERTEX= "M_TYPE_VERTEX";
constexpr std::string_view MATRIX_TYPE_PIXEL = "M_TYPE_PIXEL";
constexpr std::string_view MATRIX_TYPE_MIXED = "M_TYPE_MIXED";

namespace MatrixDefinitionBase {
    // Estructura que contiene todos los parámetros que se pueden pasar a los shaders.
    // Usamos esta como fuente única de datos para rellenar los diferentes constant buffers.
    struct MatrixParams {
        MatrixParams()
            : worldMatrix(DirectX::XMMatrixIdentity()),
            viewMatrix(DirectX::XMMatrixIdentity()),
            projectionMatrix(DirectX::XMMatrixIdentity()),
            lightViewProjectionMatrix(DirectX::XMMatrixIdentity()),
            cameraPosition(0.0f, 0.0f, 0.0f),
            lightDirection(0.0f, -1.0f, 0.0f), // Dirección por defecto de la luz (hacia abajo)
            lightColor(1.0f, 1.0f, 1.0f, 1.0f), // Color por defecto de la luz (blanco)
            materialAlbedo(0.8f, 0.8f, 0.8f, 1.0f), // Color base del material (si no hay textura)
            materialRoughness(0.5f),           // Rugosidad del material (0.0=liso, 1.0=rugoso)
            materialMetallic(0.0f),            // Metalicidad del material (0.0=dieléctrico, 1.0=metal)
            materialF0(0.04f, 0.04f, 0.04f),    // Reflectividad especular para dieléctricos (generalmente 0.04)
            materialAO(0.3f),                 // Oclusión ambiental (1.0=sin oclusión)
            paddingCamera(.0f),
            paddingLight1(.0f),
            paddingMaterial1(.0f),
            paddingMaterial2(.0f)
        {
        }

        DirectX::XMMATRIX worldMatrix;
        DirectX::XMMATRIX viewMatrix;
        DirectX::XMMATRIX projectionMatrix;

        // Datos para CameraData
        DirectX::XMFLOAT3 cameraPosition;
        float paddingCamera; // Relleno para alinear a 16 bytes (después de XMFLOAT3)

        // Datos para DirectionalLight
        DirectX::XMFLOAT3 lightDirection;
        float paddingLight1; // Relleno
        DirectX::XMFLOAT4 lightColor; // El color de la luz direccional

        // Datos para MaterialData (si se usan como constantes en lugar de texturas)
        DirectX::XMFLOAT4 materialAlbedo; // Albedo (color base) del material
        float materialRoughness;      // Rugosidad del material
        float materialMetallic;       // Metalicidad del material
        DirectX::XMFLOAT3 materialF0;         // F0 para dieléctricos (o se calcula para metales)
        float materialAO;             // Oclusión ambiental
        float paddingMaterial1;       // Relleno para asegurar alineación final a 16 bytes si es necesario
        float paddingMaterial2;

		// Transformaciones de texturas (si se usan)
        DirectX::XMFLOAT4 textureTransform; // (x, y: escala / z,w: traslación)

		// Datos para matrices de sombras
        DirectX::XMMATRIX lightViewProjectionMatrix;
    };
}