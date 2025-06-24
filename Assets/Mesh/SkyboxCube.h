// SkyboxCube.h
#pragma once
#include <vector>
#include <cstdint>
#include "Material/includes/VertexDefinition.h"

namespace SkyboxCube {
    inline std::vector<VertexDefinition::SimpleVertex> GetVertex() {
        return {
            {{-1,  1, -1}}, {{1,  1, -1}}, {{1, -1, -1}}, {{-1, -1, -1}}, // Frente (-Z)
            {{-1,  1,  1}}, {{1,  1,  1}}, {{1, -1,  1}}, {{-1, -1,  1}}, // Atrás (+Z)
        };
    }

    inline std::vector<uint16_t> GetIndex() {
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