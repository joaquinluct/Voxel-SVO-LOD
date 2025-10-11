#include "AggregatedMeshChunk.h"
#include <algorithm>
#include <Windows.h>
#include <memory>
#include <Util/RayTracing/RayTracing.h>

AggregatedMeshChunk::AggregatedMeshChunk(TerrainChunk::ChunkID id, float chunkSize)
    : m_id(id), m_chunkSize(chunkSize), m_geometryEngine(nullptr) {
}

HRESULT AggregatedMeshChunk::Init() {
    // ... (Tu lógica de Init de Chunk) ...
    return S_OK;
}

void AggregatedMeshChunk::UpdateLOD(float distance, int maxLODs) {
    // 1. Definir los umbrales de distancia (configuración del TerrainConfig)
    // Esto debería venir de los 'geometry_lod_thresholds' de tu YAML.
    // Usaremos un ejemplo de umbral fijo:

    int newLOD = 0;
    if (distance > 500.0f) newLOD = 1;
    if (distance > 1000.0f) newLOD = 2;
    if (distance > 2000.0f) newLOD = 3;

    newLOD = (std::min)(newLOD, maxLODs);

    // 2. Comprobar si se requiere reconstrucción
    if (newLOD != m_currentLOD) {
        m_currentLOD = newLOD;
        m_dirty = true; // Se requiere que el Budget System lo regenere
    }
    else {
        m_dirty = false;
    }
}

void AggregatedMeshChunk::GenerateMesh(class TerrainGeometryFactory* factory) {
    // El motor de geometría DEBE estar inyectado y el chunk DEBE estar dirty.
    if (!m_geometryEngine || !m_dirty) return;

    m_meshData = std::make_unique<TerrainChunkMeshData>();

    // **DELEGACIÓN CRUCIAL:** Se llama al motor de geometría para que 
    // genere la data del chunk en el LOD actual y lo guarde en m_meshData.

    /*
    m_geometryEngine->GenerateChunkMeshData(
        m_meshData->vertices,
        m_meshData->indices,
        m_id,
        m_chunkSize,
        m_currentLOD
    );
    */

    // Lógica posterior a la generación (cálculo de normales, etc.)
    // ...

    m_dirty = false;
}

// Implementaciones de IChunk faltantes
DirectX::XMFLOAT3 AggregatedMeshChunk::GetCenter() const {
    // TODO: Calculate proper center based on chunk position
    // For now, return a default position
    return DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
}

const Util::BoundingBox& AggregatedMeshChunk::GetBoundingBox() const {
    // TODO: Implement proper bounding box calculation based on chunk data
    return m_boundingBox;
}

bool AggregatedMeshChunk::IsInFrustum(const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes) const {
    // TODO: Implement proper frustum culling
    // For now, always return true (visible)
    return true;
}

float AggregatedMeshChunk::GetDistanceToCamera(std::shared_ptr<ICamera> camera) const {
    // TODO: Calculate distance to camera properly
    // For now, return a default distance
    return 100.0f;
}
