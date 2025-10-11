#define NOMINMAX
#include "DefaultChunkFactory.h"
#include <Game/Systems/Terrain/Engines/Chunk/AggregatedMeshChunk.h>
#include <Defines/TerrainChunk.h>

std::unique_ptr<IChunk> DefaultChunkFactory::CreateChunk(const TerrainChunk::ChunkID& id, float chunkSize) {
    // Default to aggregated mesh chunk for now
    return std::make_unique<AggregatedMeshChunk>(id, chunkSize);
}
