#pragma once

#include "MatrixDefinitionBase.h" // Incluimos la base para MatrixParamsss
#include <Defines/Enums/Matrix.h>
#include <DirectXMath.h>
#include <map>
#include <string>

using namespace MatrixDefinitionBase;

namespace CameraMatrix {
    // --- Constant Buffer para Datos de Cámara ---
    // Corresponde a 'cbuffer CameraBuffer : register(b2)' en el HLSL
    struct CameraData {
        DirectX::XMFLOAT3 CameraPosition; // Posición de la cámara en espacio mundo

        void SetCameraPosition(DirectX::XMFLOAT3 cameraPosition) {
            CameraPosition = cameraPosition;
        }

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MatrixParams> baseParams = GetMatrixParams<MatrixParams>(params["BaseParams"]);
            // Asigna la posición de la cámara desde MatrixParams
            this->CameraPosition = baseParams->cameraPosition;
        }

        UINT Size() {
            return (sizeof(CameraPosition));
        }

        MatrixBufferTypeEnum BufferType() {
            return MatrixBufferTypeEnum::Dynamic;
        }

        std::string MatrixType() {
            return MATRIX_TYPE_PIXEL.data();
        }
    };
}
