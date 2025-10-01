#pragma once

#include <Defines/Texture.h>
#include <Defines/TimeDefinition.h>
#include <Defines/WaterDefinition.h>
#include <DirectXMath.h>
#include <memory>
#include <string_view>

inline constexpr std::string_view MATRIX_TYPE_VERTEX = "M_TYPE_VERTEX";
inline constexpr std::string_view MATRIX_TYPE_PIXEL = "M_TYPE_PIXEL";
inline constexpr std::string_view MATRIX_TYPE_MIXED = "M_TYPE_MIXED";

namespace MatrixDefinitionBase {

    struct SkyboxData {
        DirectX::XMFLOAT4 skyColor;
        DirectX::XMFLOAT4 sunColor;
    };

    enum class MatrixBufferType {
        None = -1,
        woldMatrix = 0,
        viewMatrix = 1,
        projectionMatrix = 2,
        viewProjectionMatrix = 3,
        projectionOrthoMatrix = 4,
        lightViewProjectionMatrix = 5,
        textureTransform = 6,
        terrainBlenderData = 7,
        terrain2BlenderData = 8,
        cameraPosition = 9,
        lightDirection = 10,
        lightColor = 11,
        materialAlbedo = 12,
        materialRoughness = 13,
        materialMetallic = 14,
        materialF0 = 15,
        materialAO = 16,
        skyboxData = 17,
        timeData = 18,
        waterData = 19,
        waterInstancingData = 20
    };

    // Estructura que contiene todos los parámetros que se pueden pasar a los shaders.
    // Usamos esta como fuente única de datos para rellenar los diferentes constant buffers.
    struct IMatrixParams {
        virtual ~IMatrixParams() = default;
    };

    struct MatrixParams : public IMatrixParams {
        MatrixParams(const MatrixParams&) = default;
        MatrixParams(MatrixParams* self) {
            this->worldMatrix = self->worldMatrix;
            this->viewMatrix = self->viewMatrix;
            this->projectionMatrix = self->projectionMatrix;
            this->viewProjectionMatrix = self->viewProjectionMatrix;
            this->projectionOrthoMatrix = self->projectionOrthoMatrix;
            this->lightViewProjectionMatrix = self->lightViewProjectionMatrix;
            this->cameraPosition = self->cameraPosition;
            this->paddingCamera = self->paddingCamera;
            this->skyboxData = self->skyboxData;
            this->timeData = self->timeData;
        }
        MatrixParams()
            : worldMatrix(DirectX::XMMatrixIdentity()),
            viewMatrix(DirectX::XMMatrixIdentity()),
            projectionMatrix(DirectX::XMMatrixIdentity()),
            viewProjectionMatrix(DirectX::XMMatrixIdentity()),
            projectionOrthoMatrix(DirectX::XMMatrixIdentity()),
            lightViewProjectionMatrix(DirectX::XMMatrixIdentity()),
            cameraPosition(0.0f, 0.0f, 0.0f),
            paddingCamera(.0f),
            skyboxData(),
            timeData()
        {
        }

        DirectX::XMMATRIX worldMatrix;
        DirectX::XMMATRIX viewMatrix;
        DirectX::XMMATRIX projectionMatrix;
        DirectX::XMMATRIX viewProjectionMatrix;
        DirectX::XMMATRIX projectionOrthoMatrix;

        // Datos para CameraData
        DirectX::XMFLOAT3 cameraPosition;
        float paddingCamera; // Relleno para alinear a 16 bytes (después de XMFLOAT3)


        // Datos para matrices de sombras
        DirectX::XMMATRIX lightViewProjectionMatrix;

        // Datos para SkyboxData
        SkyboxData skyboxData;

        // Datos del tiempo
        TimeDefinition::TimeData timeData;

    };

    struct MaterialMatrixParams : public IMatrixParams {
        MaterialMatrixParams(const MaterialMatrixParams&) = default;
        MaterialMatrixParams(MaterialMatrixParams* self) {
            this->terrain2BlenderData = self->terrain2BlenderData;
            this->terrainBlenderData = self->terrainBlenderData;
            this->materialAlbedo = self->materialAlbedo;
            this->materialAO = self->materialAO;
            this->materialF0 = self->materialF0;
            this->materialMetallic = self->materialMetallic;
            this->materialRoughness = self->materialRoughness;
            this->textureTransform = self->textureTransform;
        };
        MaterialMatrixParams()
            : materialAlbedo(1.0f, 1.0f, 1.0f, 1.0f),
            materialRoughness(1.0f),
            materialMetallic(0.0f),
            materialF0(0.04f, 0.04f, 0.04f), // Valor típico para dieléctricos
            materialAO(1.0f),
            textureTransform(1.0f, 1.0f, 0.0f, 0.0f),
            terrainBlenderData(),
            terrain2BlenderData()
        {
        };
        // Datos para MaterialData (si se usan como constantes en lugar de texturas)
        DirectX::XMFLOAT4 materialAlbedo; // Albedo (color base) del material
        float materialRoughness;      // Rugosidad del material
        float materialMetallic;       // Metalicidad del material
        DirectX::XMFLOAT3 materialF0; // F0 para dieléctricos (o se calcula para metales)
        float materialAO;             // Oclusión ambiental        
        // Transformaciones de texturas (si se usan)
        DirectX::XMFLOAT4 textureTransform; // (x, y: escala / z,w: traslación)
        // Mezcla de texturas para el terreno
        TextureDefines::CBTerrainBlendBuffer terrainBlenderData; // Datos de mezcla de texturas para terreno;
        TextureDefines::CBTerrain2BlendBuffer terrain2BlenderData; // Datos de mezcla de texturas para terreno;
    };

    struct LightMatrixParams : public IMatrixParams {
        LightMatrixParams() = default;
        LightMatrixParams(LightMatrixParams*) : lightColor(), lightDirection() {
            this->lightDirection = lightDirection;
            this->lightColor = lightColor;
            this->fogStartDistance = fogStartDistance;
            this->fogEndDistance = fogEndDistance;
            this->fogColor = fogColor;
            this->fogHeightFalloff = fogHeightFalloff;
            this->fogDensity = fogDensity;
        }
        LightMatrixParams(const LightMatrixParams&) = default;
        //      LightMatrixParams()
        //          : lightDirection(0.0f, -1.0f, 0.0f), // Luz direccional apuntando hacia abajo por defecto
        //          lightColor(1.0f, 1.0f, 1.0f, 1.0f) // Luz blanca por defecto
        //      {
              //}
              // 
        // Datos para DirectionalLight
        DirectX::XMFLOAT3 lightDirection;
        DirectX::XMFLOAT4 lightColor; // El color de la luz direccional
        // Datos para niebla
        float fogStartDistance;         // Distancia donde la niebla empieza a aparecer (ej: 1000.0f)
        float fogEndDistance;           // Distancia donde la niebla es 100% opaca (ej: 3000.0f)
        float fogHeightFalloff;   // Controla cómo la altura afecta la densidad de la niebla (no siempre se usa)
        DirectX::XMFLOAT3 fogColor;     // Color de la niebla (El gris que quieres en el horizonte)
        float fogDensity;               // Se usa para calcular niebla más avanzada, aquí lo dejamos en float.
    };

    struct WaterMatrixParams : public IMatrixParams {
        WaterMatrixParams() = default;
        WaterMatrixParams(WaterMatrixParams* self) {
            this->waterData = self->waterData;
            this->waterInstancingData = self->waterInstancingData;
        }
        WaterMatrixParams(const WaterMatrixParams&) = default;
        // Datos para el mar
        WaterDefinition::WaterData waterData;
        WaterDefinition::WaterInstancing waterInstancingData;
    };

    template<typename T>
    std::shared_ptr<T> GetMatrixParams(std::shared_ptr<IMatrixParams> params) {
        return std::dynamic_pointer_cast<T>(params);
    }
}
