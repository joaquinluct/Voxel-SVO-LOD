#pragma once

#include <Defines/TerrainChunk.h>
#include <memory>
#include <DirectXMath.h>

class IChunk;
class ProceduralEngineBase;

// Minimal factory interface for creating IChunk instances.
class IChunkFactory {
public:
    virtual ~IChunkFactory() = default;
    // Create a chunk for the given id and chunkSize. Ownership is transferred.
    virtual std::unique_ptr<IChunk> CreateChunk(const TerrainChunk::ChunkID& id, float chunkSize) = 0;
};
