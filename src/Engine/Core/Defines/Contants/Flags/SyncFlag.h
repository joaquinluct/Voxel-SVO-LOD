#pragma once

enum class SyncFlagIndex : size_t {
    Initialized,
    HasScene,
    ChunkVisible,
    ChunkWithVertex,
    ChunkWithVertexBuffer,
    HasTerrain,
    HasBiome,
    HasConstsBufferDefined,
    HasConstsBufferBinded,

    Count // Siempre al final
};


