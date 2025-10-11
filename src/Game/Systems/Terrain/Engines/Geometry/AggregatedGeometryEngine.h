#pragma once
#include <Game/Systems/Terrain/Defines/TerrainGeometryEngineBase.h>
#include <vector>
#include <unordered_map>
#include <Windows.h>
#include <Engine/Core/Defines/VertexDefinition.h>
#include <Defines/TerrainChunk.h>

class AggregatedGeometryEngine : public TerrainGeometryEngineBase {
public:
    AggregatedGeometryEngine() = default;
    ~AggregatedGeometryEngine() = default;
    void Init(TerrainConfig* config) override;
    void GenerateMesh(
        std::vector<VertexDefinition::TextureMapVertex>& globalVertexPool,
        std::unordered_map<size_t, UINT>& vertexLookup,
        std::vector<UINT>& localIndices,
        const TerrainChunk::ChunkID& chunkId,
        int zone = 0) override;
    std::string GetMeshAssetName() const override;
    void Draw() override;
    
    // Método específico para renderizar chunks con mallas agregadas
    void RenderChunks(const std::vector<TerrainChunk::ChunkID>& chunkIds, const DirectX::XMFLOAT3& cameraPos);
private:
};
