#pragma once
#include <Assets/Base/MeshAssset/MeshData.h>
#include <Defines/TerrainChunk.h>
#include <Defines/VertexDefinition.h>
#include <memory>
#include <memory>

class TerrainConfig;
class ProceduralEngineBase;

using TerrainMeshData = MeshData<VertexDefinition::TextureMapVertex, UINT>;

class TerrainMeshFactory {
public:
    static TerrainMeshData Generate(
        const TerrainConfig& config,
        const TerrainChunk::ChunkID& id,
        int lodLevel,
        std::shared_ptr<ProceduralEngineBase> engine
    );
    // Deprecated: GenerateFromConfig removed. Distribution engines should
    // iterate visible chunks and call TerrainMeshFactory::Generate per-chunk.
};
