#pragma once
#include <Config/Game/GeometryEngines/GeometryChunkEngineConfig.h>
#include <Defines/TerrainChunk.h>
#include <Defines/VertexDefinition.h>
#include <Game/System/TerrainConfig.h>
#include <Game/Systems/Terrain/Defines/TerrainGeometryEngineBase.h>
#include <unordered_map>
#include <vector>
#include <Windows.h>

class GeometryChunkEngine : public TerrainGeometryEngineBase {
private:
    GeometryChunkEngineConfig config;  // Mantenemos el config como está por ahora

    size_t HashVertex(const VertexDefinition::TextureMapVertex& v) const;
public:
    GeometryChunkEngine() = default;
    ~GeometryChunkEngine() = default;

    void Init(TerrainConfig* terrainConfig) override;

    static constexpr const char* engineName = "GeometryChunkEngine";
    /*const std::string& GetServiceName() const override { return engineName; }
    static const std::string& GetStaticServiceName() { return engineName; }*/

    void GenerateMesh(
        std::vector<VertexDefinition::TextureMapVertex>& globalVertexPool,
        std::unordered_map<size_t, UINT>& vertexLookup,
        std::vector<UINT>& index,
        const TerrainChunk::ChunkID& id,
        int zone) override;

    std::string GetMeshAssetName() const override;
};
