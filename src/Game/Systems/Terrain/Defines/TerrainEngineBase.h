#pragma once
#include <Assets/Base/MeshAssetBase.h>
#include <Defines/CameraDefinition.h>
#include <Defines/TerrainChunk.h>
#include <Defines/VertexDefinition.h>
#include <DirectXMath.h>
#include <Game/System/TerrainConfig.h>
#include <Game/Systems/Terrain/Defines/Chunk/TerrainChunkConfig.h>
#include <iCamera.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>

class TerrainEngineBase
{
    virtual void Init(TerrainConfig* config) {};
    virtual void GenerateMesh(
        std::vector<VertexDefinition::TextureMapVertex>& globalVertexPool,
        std::unordered_map<size_t, UINT>& vertexLookup,
        std::vector<UINT>& index,
        const TerrainChunk::ChunkID& id,
        int zone) {
    };
    virtual std::vector<MeshAssetBase*> GetVisibleMeshes(std::vector<CameraDefinition::FrustumPlane>& frustumPlanes, int zone = 0) { return {}; }
    virtual void UpdateTerrain(const DirectX::XMFLOAT3& worldPosition) {};
    virtual void Generate(ICamera* camera) {}
    virtual TerrainChunkConfig* GetChunkConfig() { return {}; }

    virtual bool IsDirty() { return false; }
    virtual void SetDirty(bool dirty) {}

    virtual bool IsGeneratingDistribution() { return false; }
    virtual void SetIsGeneratingDistribution(bool isGenerating) {}

    virtual std::string GetMeshAssetName() const { return ""; }
};
