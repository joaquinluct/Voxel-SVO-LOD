// SkyboxCube.h
#pragma once
#include <vector>
#include <cstdint>
#include "Defines/VertexDefinition.h"
#include <Assets/Base/MeshAsset.h>
#include <Assets/IAssetMesh.h>

class SkyboxCube : public IAssetMesh<VertexDefinition::SimpleVertex>{
public:
    SkyboxCube() = default;
    //~SkyboxCube() override {};
    const std::string& GetAssetName() {
        static const std::string assetName = "SkyboxCube";
        return assetName;
    }

    std::vector<VertexDefinition::SimpleVertex> GetVertex() {
        return {
            {-1.0f,  1.0f, -1.0f}, {1.0f,  1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, // Frente (-Z)
            {-1.0f,  1.0f,  1.0f}, {1.0f,  1.0f,  1.0f}, {1.0f, -1.0f,  1.0f}, {-1.0f, -1.0f,  1.0f}, // Atrás (+Z)
        };
    }

    std::vector<uint16_t> GetIndex() {
        return {  // Cara frontal
            0, 1, 2, 0, 2, 3,
            // Cara trasera
            5, 4, 7, 5, 7, 6,
            // Cara izquierda
            4, 0, 3, 4, 3, 7,
            // Cara derecha
            1, 5, 6, 1, 6, 2,
            // Cara superior
            4, 5, 1, 4, 1, 0,
            // Cara inferior
            3, 2, 6, 3, 6, 7
        };
    };
};