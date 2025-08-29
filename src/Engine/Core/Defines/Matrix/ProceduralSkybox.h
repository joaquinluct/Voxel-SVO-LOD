#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <map>
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

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MatrixParams> baseParams = GetMatrixParams<MatrixParams>(params["BaseParams"]);
            std::shared_ptr<LightMatrixParams> lightParams = GetMatrixParams<LightMatrixParams>(params["LightParams"]);
            
			this->skyColor = baseParams->skyboxData.skyColor;
			this->sunColor = baseParams->skyboxData.sunColor;
            this->lightDirection = DirectX::XMFLOAT4{ lightParams->lightDirection.x, lightParams->lightDirection.y, lightParams->lightDirection.z, 1.0f };
            this->lightColor = lightParams->lightColor;
        }

        UINT Size() {
            return sizeof(skyColor) + sizeof(sunColor) + sizeof(lightDirection) + sizeof(lightColor);
        }

        std::string MatrixType() {
            return MATRIX_TYPE_PIXEL.data();
        }
    };
}