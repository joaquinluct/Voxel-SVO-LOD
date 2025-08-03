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

    // Necesario para el cálculo de sombras en el shader principal.
    struct LightSpaceMatrices {
		DirectX::XMMATRIX worldMatrix; // Matriz de transformación del objeto en espacio mundo
        DirectX::XMMATRIX LightViewProjection; // Matriz de vista*proyección de la luz

        void SetMatrixData(MatrixParams params) {
            // Asigna la matriz de vista-proyección de la luz desde MatrixParams
            // DirectX::XMMATRIX ya es de 16 bytes de alineación y tamaño apropiado.
            this->LightViewProjection = params.lightViewProjectionMatrix;
        }

        UINT Size() {
            // DirectX::XMMATRIX ya está alineada y es un tamaño apropiado para un constant buffer (64 bytes).
            return 2 * sizeof(DirectX::XMMATRIX);
        }

        std::string MatrixType() {
            // Normalmente esta matriz es usada tanto en el Vertex Shader (para transformar posiciones)
            // como en el Pixel Shader (para muestrear el mapa de sombras).
            return MATRIX_TYPE_VERTEX.data();
        }
    };
}