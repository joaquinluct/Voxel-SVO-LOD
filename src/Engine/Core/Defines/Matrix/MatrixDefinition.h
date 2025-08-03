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

        void SetMatrixData(MatrixParams params) {
            this->worldMatrix = params.worldMatrix;
            this->viewMatrix = params.viewMatrix;
            this->projectionMatrix = params.projectionMatrix;
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

        void SetMatrixData(MatrixParams params) {
            this->viewMatrix = params.viewMatrix;
            this->projectionMatrix = params.projectionMatrix;
        }

        UINT Size() {
            return sizeof(viewMatrix) + sizeof(projectionMatrix);
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

        void SetMatrixData(MatrixParams params) {
            this->projectionMatrix = params.projectionMatrix;
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
        MatrixBufferTypeOrthographic,           // Matriz Projection (para UI, etc.)
        Light::DirectionalLight,                // Datos de luz direccional para PBR
		Light::LightSpaceMatrices,              // Matrices de espacio de luz para sombras
        CameraMatrix::CameraData,               // Posición de la cámara
        MaterialMatrix::MaterialData,           // Propiedades de material PBR
		MaterialMatrix::TextureTransformations  // Transformaciones de texturas para PBR
    >;

    // Declaración de la función Get para crear instancias de los buffers
    static AnyMatrixBuffer Get(std::string matrixDefinitionName) {
        if (matrixDefinitionName == "MatrixBufferType") {
            return MatrixBufferType{};
        }
        if (matrixDefinitionName == "MatrixBufferTypeSkyBox") {
            return MatrixBufferTypeSkyBox{};
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
        if (matrixDefinitionName == "LightSpaceMatrices") {
            return Light::LightSpaceMatrices{};
        }
        throw std::runtime_error("Unknown matrix definition name: " + matrixDefinitionName);
    }

} // namespace MatrixDefinition