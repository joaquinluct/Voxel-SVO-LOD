#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include "MatrixDefinitionBase.h" // Incluimos la base para MatrixParamsss

using namespace MatrixDefinitionBase;

namespace SkyboxMatrix {
    struct ProceduralSkyboxMatrix {
        DirectX::XMFLOAT4 skyColor; // Color del cielo
        DirectX::XMFLOAT4 sunColor; 
		DirectX::XMFLOAT4 lightDirection; // Dirección de la luz
		DirectX::XMFLOAT4 lightColor; // Color de la luz
        
        ProceduralSkyboxMatrix() :
            skyColor(DirectX::XMFLOAT4{ .4f, .4f, 1.0f, 1.0f }),
			sunColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 0.8f, 1.0f }), // Color del sol
			lightDirection(DirectX::XMFLOAT4{ 0.0f, -1.0f, 0.0f, 0.0f }), // Dirección de la luz hacia abajo
            lightColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f }) // Color de la luz blanca
        {
		}

        void SetMatrixData(MatrixParams params) {
            this->skyColor = params.skyboxData.skyColor;
			this->sunColor = params.skyboxData.sunColor;
            this->lightDirection = DirectX::XMFLOAT4{ params.lightDirection.x, params.lightDirection.y, params.lightDirection.z, 1.0f };
			this->lightColor = params.lightColor;
        }

        UINT Size() {
            return sizeof(skyColor) + sizeof(sunColor) + sizeof(lightDirection) + sizeof(lightColor);
        }

        std::string MatrixType() {
            return MATRIX_TYPE_PIXEL.data();
        }
    };
}