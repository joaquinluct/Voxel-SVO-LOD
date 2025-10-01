#pragma once

#include <FastNoiseLite/FastNoiseLite.h>
#include <string>
#include <vector>

// Archivo: Defines/AreaMapping.h (Refinado)

// Archivo: Defines/AreaMapping.h

struct NoiseParameters {
    FastNoiseLite::NoiseType noiseType = FastNoiseLite::NoiseType_Perlin;
    FastNoiseLite::FractalType fractalType = FastNoiseLite::FractalType_FBm;
    float frequency = 0.01f;
    int octaves = 4;
    float lacunarity = 2.0f;
    float gain = 0.5f;
};

struct AreaTypeDefinition {
    // 2) Nombre y 4) Bioma asociado
    std::string name;
    int biomeId;
    // 3) Color del área
    float color[4];

    // 1) Parámetros de Altura y control de Acantilados
    float baseHeightOffset;    // Altura base (ej. nivel del mar, o base de la meseta)
    float minHeightConstraint; // Altura mínima ABSOLUTA permitida (ej. 0.0)
    float maxHeightConstraint; // Altura máxima ABSOLUTA permitida (ej. 800.0)

    // Configuración del Generador (Propio para cada bioma)
    float noiseAmplitude; // Multiplicador final del ruido (ej. 100.0f)
    NoiseParameters noiseConfig;
};

struct AreaMappingDefinition {
    int seed;
    // 1) Escala global para el tamaño promedio de las áreas (manchas)
    float globalAreaScale = 0.0008f;
    float blendDistance = 100.0f;    // Distancia de mezcla (ancho de la rampa)
    std::vector<AreaTypeDefinition> areaTypes;
};
