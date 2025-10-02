#pragma once

#include <string_view>
#include <Windows.h>

inline constexpr std::string_view TEXTURE_TYPE_JPG = "JPG";
inline constexpr std::string_view TEXTURE_TYPE_JPG_ARRAY = "JPG_ARRAY";
inline constexpr std::string_view TEXTURE_TYPE_CUBEMAP = "CUBEMAP";

inline constexpr std::string_view TEXTURE_MAP_ALBEDO = "albedo";
inline constexpr std::string_view TEXTURE_MAP_NORMAL = "normal";
inline constexpr std::string_view TEXTURE_MAP_ROUGHNESS = "roughness";
inline constexpr std::string_view TEXTURE_MAP_METALLIC = "metallic";
inline constexpr std::string_view TEXTURE_MAP_AO = "ao";

namespace TextureDefines
{
    struct CBTerrainBlendBuffer
    {
        float grassHeight;
        float dirtHeight;
        float snowHeight;
        float slopeStart;
        float slopeEnd;
        float terrainScale;
    };
    struct CBTerrain2BlendBuffer
    {
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
    };

    struct TerrainBiomeBufferData
    {
        // --- Capa de Mezcla COSTERA / BAJA (reemplaza 'beachTransition' anterior)
        UINT coastalBaseIndex;     // baseAtlasIndex
        float coastalTransitionH;  // transitionHeight
        float coastalTransitionB;  // transitionBlendSlope
        float paddingCoastal;      // Relleno

        // --- Capa de Mezcla LLANURA / MEDIA (reemplaza 'grassTransition' anterior)
        UINT plainBaseIndex;       // baseAtlasIndex
        float plainTransitionH;    // transitionHeight
        float plainTransitionB;    // transitionBlendSlope
        float paddingPlain;        // Relleno

        // --- Capa de Mezcla MONTAÑA / ALTA (reemplaza 'rockTransition' y 'snowTransition')
        UINT mountainBaseIndex;    // baseAtlasIndex
        float mountainTransitionH; // transitionHeight
        float mountainTransitionB; // transitionBlendSlope
        float paddingMountain;     // Relleno

        // --- Parámetros de Mezcla basados en PENDIENTE
        float rockTransitionSlope; // Mezcla por pendiente (independiente de altura)
        float snowTransitionSlope; // Mezcla por pendiente (ej. nieve en laderas)

        // Parámetro de escala de textura global
        float terrainScale;
        float paddingGlobal;
    };

}

