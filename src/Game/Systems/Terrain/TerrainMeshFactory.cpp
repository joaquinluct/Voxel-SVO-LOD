#include "TerrainMeshFactory.h"
#include <Config/Game/System/TerrainConfig.h>
#include <Defines/Procedural.h>
#include <Defines/TerrainChunk.h>
#include <Game/Systems/Terrain/Chunk/Factory/ChunkMeshGenerator.h>
#include <Game/Systems/Terrain/LODManager.h> 
#include <Game/Systems/Terrain/ProceduralService.h>
#include <memory>
#include <string>
#include <utility>

TerrainMeshData TerrainMeshFactory::Generate(
    const TerrainConfig& config,
    const TerrainChunk::ChunkID& id,
    int lodLevel,
    std::shared_ptr<ProceduralEngineBase> engine
) {
    const std::string& generationType = config.generation_type;
    Procedural::ProceduralEngine procEngine = static_cast<Procedural::ProceduralEngine>(config.generation_algorithm);

    if (generationType == "Procedural") {
        if (procEngine == Procedural::ProceduralEngine::FastNoiseLite) {
            return ChunkMeshGenerator::GenerateMesh(
                id,
                static_cast<float>(config.chunk_size),
                lodLevel,
                config.terrainHeight,
                engine
            );
        }
        // Aquí puedes añadir más algoritmos:
        // else if (algorithm == "DoubleLayerNoise") { ... }
        // else if (algorithm == "BiomeEngine") { ... }
    }

    // Si no se reconoce el tipo, devolver malla vacía
    return TerrainMeshData{};
}

// GenerateFromConfig removed: distribution engines should iterate visible
// chunks and call TerrainMeshFactory::Generate per-chunk.

