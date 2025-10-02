#pragma once
#include "Material/PbrTextureSet.h"
#include <Windows.h>

struct BiomeTerrainDefinition {
    // === 5. JUEGOS DE TEXTURAS PBR POR ALTURA, AHORA CON MULTIPLES OPCIONES ===

    // 5.1) Texturas de Costa (Baja Altura)
    // Puede haber varias opciones de 'arena' o 'grava'
    PbrTextureSet coastalSets[3]; // Permitimos hasta 3 sets diferentes para la costa
    UINT numCoastalSets; // Cuántos sets de los 3 posibles realmente usamos
    float paddingCoastal[3]; // Relleno para alinear

    // 5.2) Texturas de Llanura / Planicie (Media Altura)
    // Puede haber varias opciones de 'hierba' o 'tierra'
    PbrTextureSet plainSets[3]; // Hasta 3 sets diferentes para la llanura
    UINT numPlainSets;
    float paddingPlain[3];

    // 5.3) Texturas de Montañas (Máxima Altura)
    // Puede haber varias opciones de 'roca', 'tierra montañosa', etc.
    PbrTextureSet mountainSets[3]; // Hasta 3 sets diferentes para las montañas
    UINT numMountainSets;
    float paddingMountain[3];

    // === 3. PARÁMETROS GLOBALES DE BIOMA / RUÍDO (Sin cambios) ===
    UINT minChunks;
    UINT maxChunks;
    UINT biomeSeed;
    UINT noiseEngineID;

    // === Nuevas texturas GLOBALES por bioma (Roca y Nieve del bioma) ===
    // Si un bioma tiene su propia roca o nieve específica, se define aquí.
    PbrTextureSet biomeRockSet; // Ej: Roca volcánica para un bioma de lava
    PbrTextureSet biomeSnowSet; // Ej: Nieve sucia para un bioma de tundra

    // Relleno final para el cbuffer completo
    float paddingFinal[8]; // Asegurar que todo el cbuffer se alinea a 16 bytes
};
