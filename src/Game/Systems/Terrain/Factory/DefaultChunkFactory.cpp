#define NOMINMAX
#include "DefaultChunkFactory.h"
#include <Game/Systems/Terrain/Engines/Chunk/AggregatedMeshChunk.h>
#include <Defines/TerrainChunk.h>

std::unique_ptr<IChunk> DefaultChunkFactory::CreateChunk(const TerrainChunk::ChunkID& id, float chunkSize) {
    // SOLUCIÓN MÍNIMA: Usar new directamente en lugar de make_unique para evitar template issues
    AggregatedMeshChunk* chunk = new AggregatedMeshChunk(id, chunkSize);
    return std::unique_ptr<IChunk>(chunk);
}
