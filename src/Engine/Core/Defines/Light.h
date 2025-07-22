#pragma once

#include <DirectXMath.h>
#include "MatrixDefinitionBase.h" // Incluimos la base para MatrixParams

using namespace MatrixDefinitionBase;

namespace Light {
    // --- Constant Buffer para Luz Direccional (PBR) ---
    // Corresponde a 'cbuffer DirectionalLightBuffer : register(b1)' en el HLSL
    struct DirectionalLight {
        DirectX::XMFLOAT3 Direction; // Dirección de la luz (vector normalizado que apunta HACIA la luz)
        float Padding1;              // Relleno para alinear a 16 bytes
        DirectX::XMFLOAT4 Color;     // Color e intensidad de la luz (ej. RGB de 0.0 a 1.0 o más para HDR)
        // No se necesitan colores ambiente/difuso/especular separados para la luz en PBR,
        // ya que estos son atributos del material y cómo interactúa con la luz.

        void SetMatrixData(MatrixParams params) {
            // Asigna los datos de luz desde MatrixParams
            this->Direction = params.lightDirection;
            this->Color = params.lightColor;
            // Asegurarse de que la dirección esté normalizada
            DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&this->Direction);
            dir = DirectX::XMVector3Normalize(dir);
            DirectX::XMStoreFloat3(&this->Direction, dir);
        }

        UINT Size() {
            // El tamaño debe ser un múltiplo de 16 bytes para los constant buffers
            return (sizeof(Direction) + sizeof(Padding1) + sizeof(Color));
        }

        std::string MatrixType() {
            return MATRIX_TYPE_PIXEL.data();
        }
    };

    // --- Constant Buffer para Datos de Cámara ---
    // Corresponde a 'cbuffer CameraBuffer : register(b2)' en el HLSL
    struct CameraData {
        DirectX::XMFLOAT3 CameraPosition; // Posición de la cámara en espacio mundo
        float Padding1;                   // Relleno para alinear a 16 bytes

        void SetMatrixData(MatrixParams params) {
            // Asigna la posición de la cámara desde MatrixParams
            this->CameraPosition = params.cameraPosition;
        }

        UINT Size() {
            return (sizeof(CameraPosition) + sizeof(Padding1));
        }

        std::string MatrixType() {
            return MATRIX_TYPE_MIXED.data();
        }
    };

    // --- Constant Buffer para Datos de Material (PBR) ---
    // Corresponde a 'cbuffer MaterialBuffer : register(b3)' en el HLSL
    // Estos valores se usan si no se cargan texturas de Albedo, Roughness, Metallic, AO.
    struct MaterialData {
        DirectX::XMFLOAT4 Albedo;       // Color base del material (RGBA)
        float Roughness;                // Rugosidad (0.0=liso, 1.0=rugoso)
        float Metallic;                 // Metalicidad (0.0=dieléctrico, 1.0=metal)
        DirectX::XMFLOAT3 F0;           // Reflectividad especular base para dieléctricos (ej. 0.04 para la mayoría)
        // Para metales, F0 se deriva del Albedo.
        float AO;                       // Oclusión ambiental (0.0=ocluso, 1.0=expuesto)
        float Padding1;                 // Relleno para alinear a 16 bytes
        float Padding2;

        void SetMatrixData(MatrixParams params) {
            // Asigna los datos de material desde MatrixParams
            this->Albedo = params.materialAlbedo;
            this->Roughness = params.materialRoughness;
            this->Metallic = params.materialMetallic;
            this->F0 = params.materialF0;
            this->AO = params.materialAO;
        }

        UINT Size() {
            // Asegurarse de que el tamaño total sea un múltiplo de 16 bytes.
            // sizeof(XMFLOAT4) = 16
            // sizeof(float) = 4
            // sizeof(XMFLOAT3) = 12
            // 16 + 4 + 4 + 12 + 4 + 4 + 4 = 48 bytes (Múltiplo de 16)
            return (sizeof(Albedo) + sizeof(Roughness) + sizeof(Metallic) + sizeof(F0) + sizeof(AO) + sizeof(Padding1) + sizeof(Padding2));
        }

        std::string MatrixType() {
            return MATRIX_TYPE_PIXEL.data();
        }
    };
}