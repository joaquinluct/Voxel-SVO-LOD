#pragma once

#include <DirectXMath.h>
#include <string>
#include "MatrixDefinitionBase.h" // Incluimos la base para MatrixParams
#include <wtypes.h>
#include <Defines/Enums/Matrix.h>

using namespace MatrixDefinitionBase;

namespace WaterMatrices
{
    // --- Constant Buffer para los parámetros del agua ---
    // Corresponde a 'cbuffer WaterBuffer : register(b5)' en el HLSL
    struct WaterData
    {
        float waveHeight;
        float waveSpeed;
        float waveLength;
        float waveFrequency;
        float waterDensity;
        float waterViscosity;
        float reflectionIntensity;
        float refractionIndex;
        float foamAmount;
        float causticsIntensity;
        float padding1; // Relleno para que el total sea múltiplo de 16 bytes
        float padding2; // Relleno para que el total sea múltiplo de 16 bytes
        DirectX::XMFLOAT4 waterColor;
        BOOL isUnderwater; // BOOL para alineación de 4 bytes
        BOOL hasFoam;
        BOOL hasCaustics;
        BOOL isReflective;
        BOOL isRefractive;
        DirectX::XMFLOAT3 padding; // Relleno para que el total sea múltiplo de 16 bytes

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<WaterMatrixParams> wParams = GetMatrixParams<WaterMatrixParams>(params["WaterParams"]);
            this->waveHeight = wParams->waterData.waveHeight;
            this->waveSpeed = wParams->waterData.waveSpeed;
            this->waveLength = wParams->waterData.waveLength;
            this->waveFrequency = wParams->waterData.waveFrequency;
            this->waterDensity = wParams->waterData.waterDensity;
            this->waterViscosity = wParams->waterData.waterViscosity;
            this->reflectionIntensity = wParams->waterData.reflectionIntensity;
            this->refractionIndex = wParams->waterData.refractionIndex;
            this->foamAmount = wParams->waterData.foamAmount;
            this->causticsIntensity = wParams->waterData.causticsIntensity;
            this->waterColor = wParams->waterData.waterColor;
            this->isUnderwater = wParams->waterData.isUnderwater;
            this->hasFoam = wParams->waterData.hasFoam;
            this->hasCaustics = wParams->waterData.hasCaustics;
            this->isReflective = wParams->waterData.isReflective;
            this->isRefractive = wParams->waterData.isRefractive;
        }

        UINT Size() {
            return sizeof(WaterData);
        }

        MatrixBufferTypeEnum BufferType() {
            return MatrixBufferTypeEnum::Dynamic;
        }

        std::string MatrixType() {
            return MATRIX_TYPE_MIXED.data();
        }
        UINT Slot() {
            return 7;
        }
    };

    // --- Constant Buffer para las matrices de instancia ---
    struct WaterInstancing
    {
        // Se usa un array para pasar todas las matrices de las instancias a la GPU
        // El tamaño debe coincidir con el del shader.
        static const int MAX_INSTANCES = 256;
        DirectX::XMMATRIX instanceWorldMatrix[MAX_INSTANCES];

        // Nota: El método SetMatrixData no se usa aquí. En su lugar,
        // las matrices se copian directamente desde un vector de matrices C++
        // a este buffer de constantes en el loop de renderizado.
        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<WaterMatrixParams> waterParams = GetMatrixParams<WaterMatrixParams>(params["WaterParams"]);
            memcpy(this->instanceWorldMatrix, waterParams->waterInstancingData.instanceWorldMatrix, sizeof(DirectX::XMMATRIX) * MAX_INSTANCES);
        }

        UINT Size() {
            // El tamaño es el número máximo de instancias por el tamaño de una matriz
            return sizeof(DirectX::XMMATRIX) * MAX_INSTANCES;
        }

        MatrixBufferTypeEnum BufferType() {
            return MatrixBufferTypeEnum::Dynamic;
        }

        std::string MatrixType() {
            // Estas matrices son necesarias en el Vertex Shader para transformar la geometría
            return MATRIX_TYPE_VERTEX.data();
        }
        UINT Slot() {
            return 9;
        }
    };
} // namespace WaterMatrices
