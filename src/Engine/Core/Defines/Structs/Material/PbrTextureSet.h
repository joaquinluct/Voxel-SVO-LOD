#pragma once

#include <windows.h> 
// Defines/Structs/Material/PbrTextureSet.h

struct PbrTextureSet
{
    // El índice de la capa inicial dentro del Texture2DArray (t6)
    // donde empieza este conjunto de texturas.
    // Ej: Si baseIndex = 40, entonces:
    // Albedo = capa 40, Normal = capa 41, Roughness = capa 42, etc.
    UINT baseAtlasIndex;

    // Altura (en unidades del mundo, ej. metros) donde esta textura
    // debe empezar a ser predominante o comenzar a mezclarse con la superior.
    float transitionHeight;

    // El rango de suavidad (slope) usado para el smoothstep/lerp en el shader.
    // Un valor mayor significa una transición más suave/ancha.
    float transitionBlendSlope;

    // Relleno para que la estructura completa (16 bytes) mantenga la alineación 
    // en arrays de C++ y al usarse individualmente.
    float padding1;
};

// Tamaño total: 4 * 4 bytes = 16 bytes. Alineación garantizada para cbuffer.
