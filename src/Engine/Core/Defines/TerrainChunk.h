#pragma once

#include <DirectXMath.h>
#include <functional>
#include <optional>
#include <string>
#include <Windows.h>

namespace TerrainChunk {

    struct TerrainProceduralDefinition {
        float scale;
        int32_t octaves;
        float persistance;
        float lacunarity;
        int32_t seed;
        int32_t engine;
        float terrainHeight;
        float waterLevel;

        // Nuevos campos para las dimensiones del mundo finito
        float worldWidth;
        float worldHeight;
    };

    struct TerrainChunkDefinition {
        int32_t initialRenderDistanceChunks;
        float chunSize;
    };


    // Identificador único para cada chunk en el mundo
    struct ChunkID {
        int x, y, z;
        bool operator==(const ChunkID& other) const {
            return x == other.x && y == other.y && z == other.z;
        }
        std::string toString() const {
            return std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
        }
    };

    struct ChunkHasher {
        std::size_t operator()(const ChunkID& id) const {
            size_t h1 = std::hash<int>()(id.x);
            size_t h2 = std::hash<int>()(id.y);
            size_t h3 = std::hash<int>()(id.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    enum class NeighborDirection {
        UNDEFINED = -1,
        NORTH,
        SOUTH,
        EAST,
        WEST,
        NORTH_EAST,
        NORTH_WEST,
        SOUTH_EAST,
        SOUTH_WEST
    };

    inline static NeighborDirection GetOppositeDirection(NeighborDirection dir) {
        switch (dir) {
        case NeighborDirection::NORTH: return NeighborDirection::SOUTH;
        case NeighborDirection::SOUTH: return NeighborDirection::NORTH;
        case NeighborDirection::EAST:  return NeighborDirection::WEST;
        case NeighborDirection::WEST:  return NeighborDirection::EAST;
        default: return dir;
        }
    }

    struct NeighborPair {
        std::optional<DirectX::XMFLOAT3> lower;
        std::optional<DirectX::XMFLOAT3> upper;
    };

    struct ChunkBufferRegion {
        ChunkBufferRegion() : vertexOffset(0), indexOffset(0), indexCount(0), vertexCount(0), active(true) {}
        UINT vertexOffset;
        UINT indexOffset;
        UINT vertexCount;
        UINT indexCount;
        bool active = true;
    };

    inline size_t HashRegion(const ChunkBufferRegion& region) {
        size_t h1 = std::hash<UINT>()(region.vertexOffset);
        size_t h2 = std::hash<UINT>()(region.indexOffset);
        size_t h3 = std::hash<UINT>()(region.vertexCount);
        size_t h4 = std::hash<UINT>()(region.indexCount);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
}
