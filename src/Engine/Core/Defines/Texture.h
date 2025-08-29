#pragma once

#include <string_view>

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
}

