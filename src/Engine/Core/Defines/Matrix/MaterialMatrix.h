#pragma once

#include "MatrixDefinitionBase.h" // Incluimos la base para MatrixParamsss
#include <d3d11.h>
#include <Defines/Enums/Matrix.h>
#include <DirectXMath.h>
#include <string>

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

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MaterialMatrixParams> materialParams = GetMatrixParams<MaterialMatrixParams>(params["MaterialParams"]);
            // Asigna los datos de material desde MatrixParams
            this->Albedo = materialParams->materialAlbedo;
            this->Roughness = materialParams->materialRoughness;
            this->Metallic = materialParams->materialMetallic;
            this->marerialF0 = materialParams->materialF0;
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

        MatrixBufferTypeEnum BufferType() {
            return MatrixBufferTypeEnum::Static;
        }

        std::string MatrixType() {
            return MATRIX_TYPE_PIXEL.data();
        }
    };


    struct TextureTransformations {
        // 16 bytes
        DirectX::XMFLOAT4 TextureTransform;

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MaterialMatrixParams> materialParams = GetMatrixParams<MaterialMatrixParams>(params["MaterialParams"]);
            this->TextureTransform = materialParams->textureTransform;
        }

        UINT Size() {
            // Asegurarse de que el tamaño total sea un múltiplo de 16 bytes.
            // sizeof(XMFLOAT4) = 16
            // sizeof(float) = 4
            // sizeof(XMFLOAT3) = 12
            // 16 + 4 + 4 + 12 + 4 + 4 + 4 = 48 bytes (Múltiplo de 16)
            return (sizeof(TextureTransform));
        }

        MatrixBufferTypeEnum BufferType() {
            return MatrixBufferTypeEnum::Static;
        }

        std::string MatrixType() {
            return MATRIX_TYPE_VERTEX.data();
        }
    };

    struct TerrainBlendBuffer {
        float grassHeight;
        float dirtHeight;
        float snowHeight;
        float slopeStart;
        float slopeEnd;
        float terrainScale;
        float padding1; // Relleno para alinear a 16 bytes
        float padding2; // Relleno para alinear a 16 bytes

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MaterialMatrixParams> materialParams = GetMatrixParams<MaterialMatrixParams>(params["MaterialParams"]);
            this->grassHeight = materialParams->terrainBlenderData.grassHeight;
            this->dirtHeight = materialParams->terrainBlenderData.dirtHeight;
            this->snowHeight = materialParams->terrainBlenderData.snowHeight;
            this->slopeStart = materialParams->terrainBlenderData.slopeStart;
            this->slopeEnd = materialParams->terrainBlenderData.slopeEnd;
            this->terrainScale = materialParams->terrainBlenderData.terrainScale;
        }
        UINT Size() {
            // Asegurarse de que el tamaño total sea un múltiplo de 16 bytes.
            return (sizeof(grassHeight) + sizeof(dirtHeight) + sizeof(snowHeight) + sizeof(slopeStart) + sizeof(slopeEnd) + sizeof(terrainScale) + sizeof(padding1) + sizeof(padding2));
        }

        MatrixBufferTypeEnum BufferType() {
            return MatrixBufferTypeEnum::Static;
        }

        std::string MatrixType() {
            return MATRIX_TYPE_PIXEL.data();
        }

    };

    struct Terrain2BlendBuffer {
        float grassTransitionHeight;
        float grassTransitionSlope;
        float dirtTransitionHeight;
        float dirtTransitionSlope;

        float rockTransitionHeight;
        float rockTransitionSlope;
        float snowTransitionHeight;
        float snowTransitionSlope;

        float beachTransitionHeight;
        float beachTransitionSlope;
        float terrainScale;
        float padding1; // Relleno para alinear a 16 bytes

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MaterialMatrixParams> materialParams = GetMatrixParams<MaterialMatrixParams>(params["MaterialParams"]);
            this->grassTransitionHeight = materialParams->terrain2BlenderData.dirtTransitionHeight;
            this->grassTransitionSlope = materialParams->terrain2BlenderData.dirtTransitionSlope;
            this->dirtTransitionHeight = materialParams->terrain2BlenderData.grassTransitionHeight;
            this->dirtTransitionSlope = materialParams->terrain2BlenderData.grassTransitionSlope;
            this->rockTransitionHeight = materialParams->terrain2BlenderData.rockTransitionHeight;
            this->rockTransitionSlope = materialParams->terrain2BlenderData.rockTransitionSlope;
            this->snowTransitionHeight = materialParams->terrain2BlenderData.snowTransitionHeight;
            this->snowTransitionSlope = materialParams->terrain2BlenderData.snowTransitionSlope;
            this->beachTransitionHeight = materialParams->terrain2BlenderData.beachTransitionHeight;
            this->beachTransitionSlope = materialParams->terrain2BlenderData.beachTransitionSlope;
            this->terrainScale = materialParams->terrain2BlenderData.terrainScale;
        }
        UINT Size() {
            // Asegurarse de que el tamaño total sea un múltiplo de 16 bytes.
            return (
                sizeof(grassTransitionHeight) + sizeof(grassTransitionSlope) +
                sizeof(dirtTransitionHeight) + sizeof(dirtTransitionSlope) +
                sizeof(rockTransitionHeight) + sizeof(rockTransitionSlope) +
                sizeof(snowTransitionHeight) + sizeof(snowTransitionSlope) +
                sizeof(beachTransitionHeight) + sizeof(beachTransitionSlope) +
                sizeof(terrainScale) + sizeof(padding1));
        }
        MatrixBufferTypeEnum BufferType() {
            return MatrixBufferTypeEnum::Static;
        }
        std::string MatrixType() {
            return MATRIX_TYPE_PIXEL.data();
        }

    };


}
