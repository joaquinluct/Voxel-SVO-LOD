#pragma once

namespace WorldDefinition
{
    enum class Type
    {
        None = 0,
        Overworld = 1 << 0, // 1
        Dungeon = 1 << 1,   // 2
        Cutscene = 1 << 2, // 4
        Minigame = 1 << 3, // 8
        Event = 1 << 4,    // 16
    };

    enum class Gameflags
    {
        None = 0,
        Chronos = 1 << 0, // 1
        Terrain = 1 << 1, // 2
        Skybox = 1 << 2,  // 4
        Biome = 1 << 3,   // 8
    };

    inline bool HasCronos(const Gameflags flags)
    {
        return (static_cast<int>(flags) & static_cast<int>(Gameflags::Chronos)) != 0;
    }
    inline bool HasTerrain(const Gameflags flags)
    {
        return (static_cast<int>(flags) & static_cast<int>(Gameflags::Terrain)) != 0;
    }
    inline bool HasSkybox(const Gameflags flags)
    {
        return (static_cast<int>(flags) & static_cast<int>(Gameflags::Skybox)) != 0;
    }
    inline bool HasBiome(const Gameflags flags)
    {
        return (static_cast<int>(flags) & static_cast<int>(Gameflags::Biome)) != 0;
    }
}
