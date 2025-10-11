#pragma once
#include <Defines/TerrainChunk.h>
// BoundingBox is already included via RayTracing.h
#include <DirectXMath.h>
#include <IChunk.h>
#include <Util/RayTracing/RayTracing.h>

// Datos mínimos que el Instancing Ring Buffer necesita subir a la GPU
struct TessellationInstanceData {
    DirectX::XMFLOAT4X4 WorldMatrix; // Posición global del chunk
    float LodFactor;                 // Factor de teselación (p. ej., 4.0 a 64.0)
    float Padding[3];                // Relleno para alineación de Constant Buffer (16 bytes)
    // float HeightmapScale;          // Si es necesario, la escala de la textura de altura
};

class TessellationChunk final : public IChunk {
private:
    TerrainChunk::ChunkID m_id;
    float m_chunkSize;
    float m_currentLodFactor = 0.0f;
    bool m_dirty = true;
    Util::BoundingBox m_boundingBox;

    // NOTA: No hay std::vector<Vertex> ni MeshData.

public:
    TessellationChunk(TerrainChunk::ChunkID id, float chunkSize);

    // Implementaciones de IChunk (simplificadas para el ejemplo):
    HRESULT Init() override;
    TerrainChunk::ChunkID GetID() const override { return m_id; }
    // ... (otros getters de IChunk) ...
    const Util::BoundingBox& GetBoundingBox() const override;
    float GetChunkSize() const override { return m_chunkSize; }

    bool IsInFrustum(const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes) const override;
    float GetDistanceToCamera(std::shared_ptr<ICamera> camera) const override;

    bool IsDirty() const override { return m_dirty; }
    void SetDirty(bool dirty) override { m_dirty = dirty; }

    // Método CLAVE: Calcula el factor de LOD/Teselación
    void UpdateLOD(float distance, int maxLODs) override;
    int GetCurrentLOD() const override { /* Devuelve el factor como indicación */ return static_cast<int>(m_currentLodFactor); }

    // La data de la CPU siempre está 'lista', pues es solo información.
    bool HasCPUMeshDataReady() const override { return true; }

    // --- Específico de TessellationChunk ---
    TessellationInstanceData GetInstanceData() const;
};
