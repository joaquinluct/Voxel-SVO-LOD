#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include "MatrixDefinitionBase.h" // Incluimos la base para MatrixParamsss

using namespace MatrixDefinitionBase;

namespace CameraMatrix {
    // --- Constant Buffer para Datos de Cámara ---
    // Corresponde a 'cbuffer CameraBuffer : register(b2)' en el HLSL
    struct CameraData {
        DirectX::XMFLOAT3 CameraPosition; // Posición de la cámara en espacio mundo
        float Padding1;                   // Relleno para alinear a 16 bytes

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MatrixParams> baseParams = GetMatrixParams<MatrixParams>(params["BaseParams"]);
            // Asigna la posición de la cámara desde MatrixParams
            this->CameraPosition = baseParams->cameraPosition;
        }

        UINT Size() {
            return (sizeof(CameraPosition) + sizeof(Padding1));
        }

        std::string MatrixType() {
            return MATRIX_TYPE_MIXED.data();
        }
    };
}