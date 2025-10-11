#pragma once
#include <Game/Systems/Terrain/Factory/IChunkFactory.h>
#include <memory>

class DefaultChunkFactory : public IChunkFactory {
public:
    DefaultChunkFactory() = default;
    ~DefaultChunkFactory() override = default;

    std::unique_ptr<IChunk> CreateChunk(const TerrainChunk::ChunkID& id, float chunkSize) override;
};
