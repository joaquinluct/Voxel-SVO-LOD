#pragma once
#include <IChunk.h>
#include <Assets/Base/MeshAssset/MeshData.h>
#include <Defines/TerrainChunk.h>
#include <Defines/VertexDefinition.h>
#include <Util/RayTracing/RayTracing.h>
#include <memory>
#include <Windows.h>

using TerrainChunkMeshData = MeshData<VertexDefinition::TextureMapVertex, UINT>;

class AggregatedMeshChunk final : public IChunk {
private:
    TerrainChunk::ChunkID m_id;
    float m_chunkSize;
    int m_currentLOD = -1;
    bool m_dirty = true;
    Util::BoundingBox m_boundingBox;

    // MIEMBRO CLAVE: Almacena la malla del LOD actual
    std::unique_ptr<TerrainChunkMeshData> m_meshData;

    // Referencia al motor de geometría (para delegar la creación de V/I)
    class TerrainGeometryEngineBase* m_geometryEngine;

public:
    AggregatedMeshChunk(TerrainChunk::ChunkID id, float chunkSize);
    ~AggregatedMeshChunk() = default;

    // Implementaciones obligatorias de IChunk
    HRESULT Init() override;
    TerrainChunk::ChunkID GetID() const override { return m_id; }
    DirectX::XMFLOAT3 GetCenter() const override;
    const Util::BoundingBox& GetBoundingBox() const override;
    float GetChunkSize() const override { return m_chunkSize; }
    
    bool IsInFrustum(const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes) const override;
    float GetDistanceToCamera(std::shared_ptr<ICamera> camera) const override;
    
    bool IsDirty() const override { return m_dirty; }
    void SetDirty(bool dirty) override { m_dirty = dirty; }
    
    void UpdateLOD(float distance, int maxLODs) override;
    int GetCurrentLOD() const override { return m_currentLOD; }
    
    bool HasCPUMeshDataReady() const override { return m_meshData && !m_meshData->vertices.empty(); }

    // Específico de AggregatedMeshChunk
    void GenerateMesh(class TerrainGeometryFactory* factory);
    TerrainChunkMeshData* GetMeshData() const { return m_meshData.get(); }
    void SetGeometryEngine(class TerrainGeometryEngineBase* engine) { m_geometryEngine = engine; }
};
