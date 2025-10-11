#pragma once
#include <Assets/Base/MeshAssset/MeshData.h>
#include <Defines/TerrainChunk.h>
#include <Defines/VertexDefinition.h>
#include <memory>

class TerrainConfig;
class ProceduralEngineBase;

using TerrainMeshData = MeshData<VertexDefinition::TextureMapVertex, UINT>;

class ChunkMeshFactory {
public:
    static TerrainMeshData GenerateMesh(
        const TerrainChunk::ChunkID& id,
        float chunkSize,
        int lodLevel,
        float terrainHeight,
        std::shared_ptr<ProceduralEngineBase> engine,
        const TerrainConfig& config
    );
};
