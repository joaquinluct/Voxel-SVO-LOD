#pragma once

#include <cstdint>
#include <Defines/Procedural.h>

namespace WorldTerrain
{
    enum class WorldType
    {
        Undefined = 0,
        Finite3D = 1,
        Infinite3D = 2,
        Finite2D = 3,
        Infinite2D = 4,
    };

    enum class WorldGeometryType
    {
        Undefined = 0,
        Chunk = 1,
        Voxel = 2,
        DualContouring = 3
    }; ;

    enum class TerrainDistributionType
    {
        Undefined = 0,
        Chunk = 1,
        OctTree = 2,
        QuadTree = 3,
        Tiles = 4
    };

    enum class TerrainGeneratorType
    {
        Undefined = 0,
        HDD = 1,
        Procedural = 2,
        HeightMap = 3,
    };

    struct TerrainFiniteDefinition
    {
        float width;
        float height;
    };

    struct TerrainProceduralDefinition
    {
        int32_t seed;
        Procedural::ProceduralEngine engine;

        float terrainHeight; // Multipicador para la altura del terreno
        float waterLevel;

        float worldWidth;
        float worldHeight;
    };

    struct TerrainChunkDefinition
    {
        float chunSize;
        int32_t initialRenderDistanceChunks;
    };

    struct TerrainDefinition
    {
        //TerrainFlags flags;
        TerrainFiniteDefinition finiteDefinition;
        TerrainProceduralDefinition proceduralDefinition;
        TerrainChunkDefinition chunkDefinition;
    };
}
