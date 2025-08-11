#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include "MatrixDefinitionBase.h" // Incluimos la base para MatrixParamsss

using namespace MatrixDefinitionBase;

namespace MaterialMatrix {
    // --- Constant Buffer para Datos de Material (PBR) ---
    // Corresponde a 'cbuffer MaterialBuffer : register(b3)' en el HLSL
    // Estos valores se usan si no se cargan texturas de Albedo, Roughness, Metallic, AO.
    struct MaterialData {
        DirectX::XMFLOAT4 Albedo;       // Color base del material (RGBA)
        DirectX::XMFLOAT3 marerialF0;           // Reflectividad especular base para dieléctricos (ej. 0.04 para la mayoría)
        float Roughness;                // Rugosidad (0.0=liso, 1.0=rugoso)
        float Metallic;                 // Metalicidad (0.0=dieléctrico, 1.0=metal)
        // Para metales, F0 se deriva del Albedo.
        float AO;                       // Oclusión ambiental (0.0=ocluso, 1.0=expuesto)
        float Padding2;
        float Padding1;                 // Relleno para alinear a 16 bytes

        void SetMatrixData(MatrixParams params) {
            // Asigna los datos de material desde MatrixParams
            this->Albedo = params.materialAlbedo;
            this->Roughness = params.materialRoughness;
            this->Metallic = params.materialMetallic;
            this->marerialF0 = params.materialF0;
            this->AO = 0.7f;
        }

        UINT Size() {
            // Asegurarse de que el tamaño total sea un múltiplo de 16 bytes.
            // sizeof(XMFLOAT4) = 16
            // sizeof(float) = 4
            // sizeof(XMFLOAT3) = 12
            // 16 + 4 + 4 + 12 + 4 + 4 + 4 = 48 bytes (Múltiplo de 16)
            return (sizeof(Albedo) + sizeof(Roughness) + sizeof(Metallic) + sizeof(marerialF0) + sizeof(AO) + sizeof(Padding1) + sizeof(Padding2));
        }

        std::string MatrixType() {
            return MATRIX_TYPE_PIXEL.data();
        }
    };


    struct TextureTransformations {
        // 16 bytes
        DirectX::XMFLOAT4 TextureTransform;

        void SetMatrixData(MatrixParams params) {
            this->TextureTransform = params.textureTransform;
        }

        UINT Size() {
            // Asegurarse de que el tamaño total sea un múltiplo de 16 bytes.
            // sizeof(XMFLOAT4) = 16
            // sizeof(float) = 4
            // sizeof(XMFLOAT3) = 12
            // 16 + 4 + 4 + 12 + 4 + 4 + 4 = 48 bytes (Múltiplo de 16)
            return (sizeof(TextureTransform));
        }

        std::string MatrixType() {
            return MATRIX_TYPE_VERTEX.data();
        }
    };

    
}
