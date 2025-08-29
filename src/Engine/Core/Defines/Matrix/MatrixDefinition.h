#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <variant>
#include <string>
#include <stdexcept>
#include "Light.h"
#include "MaterialMatrix.h"
#include "CameraMatrix.h"
#include "MatrixDefinitionBase.h"
#include "ProceduralSkybox.h"
#include "WaterMatrices.h"
#include "TimeMatrices.h"

using namespace MatrixDefinitionBase;

namespace MatrixDefinition {
    // ... Tus otras estructuras MatrixBase, MatrixBufferType, MatrixBufferTypeSkyBox, MatrixBufferTypeOrthographic ...

    struct MatrixBufferType {
        DirectX::XMMATRIX worldMatrix;
        DirectX::XMMATRIX viewMatrix;
        DirectX::XMMATRIX projectionMatrix;

        MatrixBufferType() :
            worldMatrix(DirectX::XMMatrixIdentity()),
            viewMatrix(DirectX::XMMatrixIdentity()),
            projectionMatrix(DirectX::XMMatrixIdentity()) {
        }

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MatrixParams> baseParams = GetMatrixParams<MatrixParams>(params["BaseParams"]);
            this->worldMatrix = baseParams->worldMatrix;
            this->viewMatrix = baseParams->viewMatrix;
            this->projectionMatrix = baseParams->projectionMatrix;
        }

        UINT Size() {
            return sizeof(worldMatrix) + sizeof(viewMatrix) + sizeof(projectionMatrix);
        }

        std::string MatrixType() {
            return MATRIX_TYPE_VERTEX.data();
        }
    };

    struct MatrixBufferTypeSkyBox {
        DirectX::XMMATRIX viewMatrix;
        DirectX::XMMATRIX projectionMatrix;

        MatrixBufferTypeSkyBox() :
            viewMatrix(DirectX::XMMatrixIdentity()),
            projectionMatrix(DirectX::XMMatrixIdentity()) {
        }

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MatrixParams> baseParams = GetMatrixParams<MatrixParams>(params["BaseParams"]);
            this->viewMatrix = baseParams->viewMatrix;
            //this->viewMatrix = DirectX::XMMatrixTranspose(baseParams.worldMatrix);
            this->projectionMatrix = baseParams->projectionMatrix;
        }

        UINT Size() {
            return sizeof(viewMatrix) + sizeof(projectionMatrix);
        }

        std::string MatrixType() {
            return MATRIX_TYPE_VERTEX.data();
        }
    };

    struct MatrixBufferTypeProceduralSkyBox {
        DirectX::XMMATRIX viewProjectionMatrix;

        MatrixBufferTypeProceduralSkyBox() :
            viewProjectionMatrix(DirectX::XMMatrixIdentity())
        {}

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MatrixParams> baseParams = GetMatrixParams<MatrixParams>(params["BaseParams"]);
            this->viewProjectionMatrix = baseParams->viewProjectionMatrix;
        }

        UINT Size() {
            return sizeof(viewProjectionMatrix);
        }

        std::string MatrixType() {
            return MATRIX_TYPE_VERTEX.data();
        }
    };

    struct MatrixBufferTypeOrthographic {
        DirectX::XMMATRIX projectionMatrix;

        MatrixBufferTypeOrthographic() :
            projectionMatrix(DirectX::XMMatrixIdentity()) {
        }

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MatrixParams> baseParams = GetMatrixParams<MatrixParams>(params["BaseParams"]);
            this->projectionMatrix = baseParams->projectionOrthoMatrix;
        }

        UINT Size() {
            return sizeof(projectionMatrix);
        }

        std::string MatrixType() {
            return MATRIX_TYPE_VERTEX.data();
        }
    };

    // 2. Definición de AnyMatrixBuffer
    // Incluye tus nuevas estructuras de luz y material PBR.
    using AnyMatrixBuffer = std::variant<
        MatrixBufferType,                       // Matriz World, View, Projection (para la mayoría de objetos)
        MatrixBufferTypeSkyBox,                 // Matriz View, Projection (para Skybox, sin World)
        MatrixBufferTypeProceduralSkyBox,       // Matriz View, Projection (para Skybox, sin World)
        MatrixBufferTypeOrthographic,           // Matriz Projection (para UI, etc.)
        Light::DirectionalLight,                // Datos de luz direccional para PBR
        Light::LightSpaceMatrices,              // Matrices de espacio de luz para sombras
		Light::ShadowMapMatrices,               // Matrices para generar mapas de sombras
        CameraMatrix::CameraData,               // Posición de la cámara
        MaterialMatrix::MaterialData,           // Propiedades de material PBR
		MaterialMatrix::TextureTransformations, // Transformaciones de texturas para PBR
        MaterialMatrix::TerrainBlendBuffer,     // Buffer de mezcla de terreno para PBR
        MaterialMatrix::Terrain2BlendBuffer,    // Buffer de mezcla de terreno para PBR
		SkyboxMatrix::ProceduralSkyboxMatrix,   // Matriz para Procedural Skybox
        WaterMatrices::WaterData,               // Datos de agua para PBR
		WaterMatrices::WaterInstancing,         // Datos de instanciación de agua
		TimeMatrices::TimeData                  // Datos de tiempo para animaciones
    >;

    // Declaración de la función Get para crear instancias de los buffers
    static AnyMatrixBuffer Get(std::string matrixDefinitionName) {
        if (matrixDefinitionName == "MatrixBufferType") {
            return MatrixBufferType{};
        }
        if (matrixDefinitionName == "MatrixBufferTypeSkyBox") {
            return MatrixBufferTypeSkyBox{};
        }
        if (matrixDefinitionName == "MatrixBufferTypeProceduralSkyBox") {
            return MatrixBufferTypeProceduralSkyBox{};
        }
        if (matrixDefinitionName == "ProceduralSkyboxMatrix") {
            return SkyboxMatrix::ProceduralSkyboxMatrix{};
        }
        if (matrixDefinitionName == "MatrixBufferTypeOrthographic") {
            return MatrixBufferTypeOrthographic{};
        }
        if (matrixDefinitionName == "DirectionalLight") {
            return Light::DirectionalLight{};
        }
        if (matrixDefinitionName == "CameraData") {
            return CameraMatrix::CameraData{};
        }
        if (matrixDefinitionName == "MaterialData") {
            return MaterialMatrix::MaterialData{};
        }
        if (matrixDefinitionName == "TextureTransformations") {
            return MaterialMatrix::TextureTransformations{};
        }
        if (matrixDefinitionName == "TerrainBlendBuffer") {
            return MaterialMatrix::TerrainBlendBuffer{};
		}
        if (matrixDefinitionName == "Terrain2BlendBuffer") {
            return MaterialMatrix::Terrain2BlendBuffer{};
        }
        if (matrixDefinitionName == "LightSpaceMatrices") {
            return Light::LightSpaceMatrices{};
        }
        if (matrixDefinitionName == "ShadowMapMatrices") {
            return Light::ShadowMapMatrices{};
		}
        if (matrixDefinitionName == "WaterData") {
            return WaterMatrices::WaterData{};
		}
        if (matrixDefinitionName == "WaterInstance") {
            return WaterMatrices::WaterInstancing{};
        }
        if (matrixDefinitionName == "TimeData") {
            return TimeMatrices::TimeData{};
        }
        throw std::runtime_error("Unknown matrix definition name: " + matrixDefinitionName);
    }

} // namespace MatrixDefinition