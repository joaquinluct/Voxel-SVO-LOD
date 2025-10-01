#pragma once

#include "MatrixDefinitionBase.h" // Incluimos la base para MatrixParams
#include <Defines/Enums/Matrix.h>
#include <DirectXMath.h>
#include <map>
#include <memory>
#include <Windows.h>

using namespace MatrixDefinitionBase;

namespace Light {
    // --- Constant Buffer para Luz Direccional (PBR) ---
    // Corresponde a 'cbuffer DirectionalLightBuffer : register(b1)' en el HLSL
    struct __declspec(align(16)) DirectionalLight {

        // --- BLOQUE 1: LUZ DIRECCIONAL (Offset 0 - 31) ---

        // HLSL Fila 0 (Offset 0 - 15)
        DirectX::XMFLOAT3 Direction;     // float3 lightDirection (0-11 bytes)
        float PaddingLight1;             // float paddingLight1 (12-15 bytes)

        // HLSL Fila 1 (Offset 16 - 31)
        DirectX::XMFLOAT3 Color;         // float3 lightColor (16-27 bytes)
        float PaddingLight2;             // float paddingLight2 (28-31 bytes)

        // --- BLOQUE 2: NIEBLA (Offset 32 - 63) ---

        // HLSL Fila 2 (Offset 32 - 47)
        float FogStartDistance;          // float fogStartDistance
        float FogEndDistance;            // float fogEndDistance
        float FogHeightFalloff;          // float fogHeightFalloff
        float PaddingFog1;               // float paddingFog1 (Relleno para completar la fila)

        // HLSL Fila 3 (Offset 48 - 63)
        DirectX::XMFLOAT3 FogColor;      // float3 fogColor
        float FogDensity;                // float fogDensity

        // --- MÉTODOS ---

        // NOTA: La firma ha cambiado de XMFLOAT4 a XMFLOAT3 para el Color, para coincidir con HLSL.
        void SetDirectionalLight(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT4& color) {
            this->Direction = direction;
            this->Color.x = color.x;
            this->Color.y = color.y;
            this->Color.z = color.z;
            // Asegurarse de que la dirección esté normalizada
            DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&this->Direction);
            dir = DirectX::XMVector3Normalize(dir);
            DirectX::XMStoreFloat3(&this->Direction, dir);

            // Inicializar paddings
            this->PaddingLight1 = 0.0f;
            this->PaddingLight2 = 0.0f;
            this->PaddingFog1 = 0.0f;

            // Inicializar niebla con valores por defecto/seguros (puedes ajustar)
            this->FogStartDistance = 1000.0f;
            this->FogEndDistance = 3000.0f;
            this->FogHeightFalloff = 0.1f;
            this->FogColor = DirectX::XMFLOAT3(0.5f, 0.6f, 0.7f); // Gris azulado
            this->FogDensity = 50.0f; // Altura base de la niebla
        }

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            // ASUNCIÓN: Tu clase LightMatrixParams ahora contiene los campos de niebla.
            std::shared_ptr<LightMatrixParams> lightParams = GetMatrixParams<LightMatrixParams>(params["LightParams"]);

            // Asignación de datos de Luz
            this->Direction = lightParams->lightDirection;
            this->Color.x = lightParams->lightColor.x;
            this->Color.y = lightParams->lightColor.y;
            this->Color.z = lightParams->lightColor.z;

            // Asignación de datos de Niebla
            this->FogStartDistance = lightParams->fogStartDistance;
            this->FogEndDistance = lightParams->fogEndDistance;
            this->FogHeightFalloff = lightParams->fogHeightFalloff;
            this->FogColor = lightParams->fogColor;
            this->FogDensity = lightParams->fogDensity;

            // Asegurarse de que la dirección esté normalizada
            DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&this->Direction);
            dir = DirectX::XMVector3Normalize(dir);
            DirectX::XMStoreFloat3(&this->Direction, dir);

            // Rellenos
            this->PaddingLight1 = 0.0f;
            this->PaddingLight2 = 0.0f;
            this->PaddingFog1 = 0.0f;
        }

        UINT Size() {
            // El tamaño total ahora es de 64 bytes (4 filas de 16 bytes)
            return sizeof(*this);
        }

        MatrixBufferTypeEnum BufferType() {
            return MatrixBufferTypeEnum::Dynamic;
        }

        std::string MatrixType() {
            return MATRIX_TYPE_PIXEL.data();
        }

        UINT Slot() {
            return 2; // Mantiene el registro b2
        }
    };

    struct LightSpaceMatrices {
        DirectX::XMMATRIX LightViewProjection; // Matriz de vista*proyección de la luz

        void SetLightViewProjection(const DirectX::XMMATRIX& lightViewProj) {
            this->LightViewProjection = lightViewProj;
        }

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MatrixParams> baseParams = GetMatrixParams<MatrixParams>(params["BaseParams"]);
            this->LightViewProjection = baseParams->lightViewProjectionMatrix;
        }

        UINT Size() {
            return 1 * sizeof(DirectX::XMMATRIX);
        }

        MatrixBufferTypeEnum BufferType() {
            return MatrixBufferTypeEnum::Dynamic;
        }

        std::string MatrixType() {
            return MATRIX_TYPE_VERTEX.data();
        }
        UINT Slot() {
            return 5;
        }
    };

    // Necesario para el cálculo de sombras en el shader principal.
    // Esta matriz transforma las posiciones del mundo a espacio de la luz.
    // Y genera otra matriz para ser usada en el resto de shaders. (es 
    struct ShadowMapMatrices {
        DirectX::XMMATRIX worldMatrix; // Matriz de transformación del objeto en espacio mundo
        DirectX::XMMATRIX LightViewProjection; // Matriz de vista*proyección de la luz

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MatrixParams> baseParams = GetMatrixParams<MatrixParams>(params["BaseParams"]);
            // Asigna la matriz de vista-proyección de la luz desde MatrixParams
            // DirectX::XMMATRIX ya es de 16 bytes de alineación y tamaño apropiado.
            this->worldMatrix = baseParams->worldMatrix;
            this->LightViewProjection = baseParams->lightViewProjectionMatrix;
        }

        UINT Size() {
            // DirectX::XMMATRIX ya está alineada y es un tamaño apropiado para un constant buffer (64 bytes).
            return 2 * sizeof(DirectX::XMMATRIX);
        }

        MatrixBufferTypeEnum BufferType() {
            return MatrixBufferTypeEnum::Dynamic;
        }

        std::string MatrixType() {
            // Normalmente esta matriz es usada tanto en el Vertex Shader (para transformar posiciones)
            // como en el Pixel Shader (para muestrear el mapa de sombras).
            return MATRIX_TYPE_VERTEX.data();
        }
        UINT Slot() {
            return 10;
        }
    };
}
